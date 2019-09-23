
#include "kepler_mover.h"
#include "game_data.h"
#include "Eigen/Core"
#include "Eigen/Dense"
//#include <cmath>
//#include <string>

using namespace Eigen;

namespace Ign
{


static void rv2elems( const Float GM, const Eigen::Vector3d & r, const Eigen::Vector3d & v,
                      Float & a,        // Semimajor axis
                      Float & e,        // Eccentricity
                      Float & E,        // Eccentric anomaly
                      Float & I,        // Inclination
                      Float & omega,    // Argument of periapsis
                      Float & Omega,    // Longtitude of accending node
                      Float & P,        // Period
                      Float & tau,      // Periapsis crossing time
                      Eigen::Vector3d & A,
                      Eigen::Vector3d & B );
static Float speed( Float GM, Float a, Float r, bool parabolic = false );
static void velocity( const KeplerMover *km, Float & vx, Float & vy, bool parabolic = false );

// This is a special one assuming (e < 1.0).
static void ellipticInit( KeplerMover * km, Float GM, Float a, Float e, Float Omega, Float I, Float omega, Float E );

// A generic one which computes "e".
static bool genericInit( KeplerMover * km, const Eigen::Vector3d & r, const Eigen::Vector3d & v );
static void genericProcess( KeplerMover * km, Float t, Eigen::Vector3d & r, Eigen::Vector3d & v );

static void ellipticInit( KeplerMover * km, const Eigen::Vector3d & r, const Eigen::Vector3d & v );
static void ellipticProcess( KeplerMover * km, Float t, Eigen::Vector3d & r, Eigen::Vector3d & v );
static Float ellipticNextE( const Float e, const Float M, Float & E );
static Float ellipticSolveE( const Float e, const Float M, const Float E );

//static void hyperbolicInit( KeplerMover * km, Float GM, Float a, Float e, Float Omega, Float I, Float omega, Float E );
static void hyperbolicInit( KeplerMover * km, const Eigen::Vector3d & r, const Eigen::Vector3d & v );
static void hyperbolicProcess( KeplerMover * km, Float t, Eigen::Vector3d & r, Eigen::Vector3d & v );
static Float hyperbolicNextE( const Float e, const Float M, Float & expE );
static Float hyperbolicSolveE( const Float e, const Float M, const Float E );

static void parabolicInit(KeplerMover * km, const Eigen::Vector3d & r, const Eigen::Vector3d & v );
static void parabolicProcess( KeplerMover * km, Float t, Eigen::Vector3d & r, Eigen::Vector3d & v );



const Float KeplerMover::TIME_T = 60.0;
const Float KeplerMover::eps    = 1.0e-6;
const int   KeplerMover::iters  = 64;
static const Float _2PI = 2.0 * 3.1415926535;

KeplerMover::KeplerMover( Context * ctx )
    : ItemBase( ctx )
{
    active = false;
    GM = 1.0;
    active = false;
}

KeplerMover::~KeplerMover()
{
}

void KeplerMover::Start()
{
    Scene * s = GetScene();
    gameData = SharedPtr<GameData>( s->GetOrCreateComponent<GameData>() );
    if ( !gameData )
        URHO3D_LOGERROR( "Failed to get GameData instance" );
}

void KeplerMover::Update( float dt )
{
    if ( !active )
      return;

    if ( !gameData )
        return;

    // This time depending on time lapse might be mutlipled
    // by something.
    const Float dt_ = (Float)(gameData->dt) * GameData::_ONE_SECOND;

    timeLow += dt_;
    if ( timeLow > TIME_T )
    {
        timeLow -= TIME_T;
        timeHigh += TIME_T;
    }
    tau = timeLow + timeHigh;
    if ( ( P > 0.0 ) && (tau >= P) )
    {
        tau -= P;
        timeHigh -= P;
    }


    Eigen::Vector3d r, v;
    genericProcess( this, tau, r, v );

    // Compute current position and velocity.
    // Apply position to it's node.
    const Vector3 rf( r(0), r(1), r(2) );
    setR( rf );
    const Vector3 vf( v(0), v(1), v(2) );
    setV( vf );
}

void KeplerMover::launch( Float GM, Float a, Float e, Float Omega, Float I, Float omega, Float E )
{
    if ( e < (1.0 - eps) )
    {
        // Elliptic
        ellipticInit( this, GM, a, e, Omega, I, omega, E );
        active = true;
    }
}

bool KeplerMover::launch( const Vector3d & v )
{
    const Vector3d r = relR();
    const Eigen::Vector3d ev( v.x_, v.y_, v.z_ );
    const Eigen::Vector3d er( r.x_, r.y_, r.z_ );
    Eigen::Vector3d A, B;
    //rv2elems( GM, er, ev,
    //          a, e, E, I, omega, Omega, P, tau, A, B );
    const bool res = genericInit( this, er, ev );
    if ( !res )
    {
        active = false;
        return false;
    }

    timeLow  = 0.0;
    timeHigh = tau;
    active = true;
    return true;
}

bool KeplerMover::launch( const Vector3d & v, Float GM )
{
    this->GM = GM;
    launch( v );
}

void KeplerMover::stop()
{
    active = false;
}

Vector3d KeplerMover::relR() const
{
    return ItemBase::relR();
}

Vector3d KeplerMover::relV() const
{
    if ( active )
    {
        bool parabolic;
        if ( ( e > (1.0 + KeplerMover::eps) ) || ( e < (1.0 - KeplerMover::eps) ) )
            parabolic = false;
        else
            parabolic = true;
        Float c_vx, c_vy;
        velocity( this, c_vx, c_vy, parabolic );
        const Vector3d vx = ex * c_vx;
        const Vector3d vy = ey * c_vy;
        const Vector3d v = vx + vy;

        return v;
    }
    return Vector3d::ZERO;
}





static void rv2elems( const Float GM, const Eigen::Vector3d & r, const Eigen::Vector3d & v,
                      Float & a,
                      Float & e,
                      Float & E,        // Eccentric anomaly
                      Float & I,        // Inclination
                      Float & omega,
                      Float & Omega,
                      Float & P,
                      Float & tau,
                      Eigen::Vector3d & A,
                      Eigen::Vector3d & B )
{
    const Float v_2 = v.transpose() * v;
    const Float r_ = std::sqrt( r.transpose() * r );
    const Float Ws = 0.5*v_2 - GM/r_;
    a = -0.5*GM/Ws; // Semimajor axis.

    // Angular momentum.
    const Eigen::Vector3d L = r.cross( v );
    const Float L_2 = L.transpose() * L;
    // Semi-latus rectum.
    const Float p = L_2 / GM;
    // Eccentricity.
    e = std::sqrt( 1.0 - p/a );

    // Eccentric anomaly
    const Float cosE = (1.0 - r_/a);
    Float sinE = (r.transpose() * v);
    sinE = sinE / ( e * std::sqrt( GM * a ) );
    E = std::atan2( sinE, cosE );

    // Inclination
    const Float sinI = sqrt( L(0)*L(0) + L(1)*L(1) ) / std::sqrt( L_2 );
    const Float cosI = L(2) / std::sqrt( L_2 );
    I = std::atan2( sinI, cosI );

    // Argument of pericenter
    Float t1 = v(0)*L(1) - v(1)*L(0);
    Float t2 = r(2);
    const Float sinw = ( t1 / GM -
                         t2 / r_ ) / (e*sinI);

    t1 = v(2);
    t2 = L(0)*r(1) - L(1)*r(0);
    const Float cosw = ( ( std::sqrt(L_2)*t1 ) / GM - t2 / std::sqrt( L_2  * r_ ) ) / ( e*sinI );
    omega = std::atan2( sinw, cosw );

    // Longtitude of accending node
    t1 = std::sqrt( L_2 ) * sinI;
    const Float cosO =-L(1) / t1;
    const Float sinO = L(0) / t1;
    Omega = std::atan2( sinO, cosO );

    // Orbital period
    t1 = std::sqrt( a*a*a/GM );
    P = _2PI*t1;

    // Periapsis crossing time.
    tau = -( E - e*std::sin(E) ) * t1;


    A(0) = a*( std::cos(Omega)*std::cos(omega) - std::sin(Omega)*std::cos(I)*std::sin(omega) );
    A(1) = a*( std::sin(Omega)*std::cos(omega) + std::cos(Omega)*std::cos(I)*std::sin(omega) );
    A(2) = a*std::sin(I)*std::sin(omega);

    B(0) =-a*std::sqrt(1.0 - e*e)*( std::cos(Omega)*std::sin(omega) + 
                                    std::sin(Omega)*std::cos(I)*std::cos(omega) );
    B(1) = a*std::sqrt(1.0 - e*e)*(-std::sin(Omega)*std::sin(omega) + 
                                    std::cos(Omega)*std::cos(I)*std::cos(omega) );
    B(2) = a*std::sqrt(1.0 - e*e)*std::sin(I)*std::cos(omega);
}

static Float ellipticNextE( const Float e, const Float M, Float & E )
{
    Float siE = std::sin(E);
    const Float coE = std::cos(E);
    E = E - (E - e*siE - M)/(1.0 - e*coE);
    siE = std::sin(E);
    const Float err = std::abs( E - e*siE - M );
    return err;
}

static Float ellipticSolveE( const Float e, const Float M, const Float E )
{
    Float En = E;
    Float err = ellipticNextE( e, M, En );
    int n = 0;
    while ( err >= KeplerMover::eps )
    {
        err = ellipticNextE( e, M, En );

        n += 1;
        if ( n > KeplerMover::iters )
            break;
    }
    if ( En > _2PI )
        En -= _2PI;
    else if ( En < 0.0 )
        En += _2PI;
    return En;
}

static Float speed( Float GM, Float a, Float r, bool parabolic )
{
    if ( !parabolic )
    {
        const Float v = std::sqrt( GM*( 2.0/r - 1.0/a ) );
        return v;
    }
    const Float v = std::sqrt( 2.0*GM/r );
    return v;
}

static void velocity( const KeplerMover * km, Float & vx, Float & vy, bool parabolic )
{
    const Float f = km->f;
    const Float siF = std::sin(f);
    const Float coF = std::cos(f);

    const Float GM = km->GM;
    const Float a  = km->a;
    const Float l = km->l;
    const Float e = km->e;
    const Float den = 1.0 + e*coF;
    const Float r  = l/den;
    const Float dr_dTheta = l*e*siF/(den*den);
    const Float v = speed( GM, a, r, parabolic );
    const Float gamma = r / dr_dTheta;
    const Float C = v / std::sqrt( 1 + gamma*gamma );
    vx = (siF + coF*gamma)*C;
    vy = (coF - siF*gamma)*C;
}

static void ellipticInit( KeplerMover * km, Float GM, Float a, Float e, Float Omega, Float I, Float omega, Float E )
{
    km->GM = GM;
    km->a  = a;
    km->e  = e;
    km->E  = E;
    const Float n = std::sqrt( GM/(a*a*a) );
    km->P = _2PI/n;
    // Current orbital time after periapsis.
    const Float M = E - e*std::sin(E);
    const Float t = M / n;

    km->timeHigh = t;
    km->timeLow  = 0.0;


    // Determine orbit unit vectors given Omega, I, omega.
    Eigen::Vector3d ex( 1.0, 0.0, 0.0 );
    Eigen::Vector3d ey( 0.0, 1.0, 0.0 );
    const Eigen::Quaterniond qW( std::cos(Omega/2), 0.0, 0.0, std::sin(Omega/2) );
    const Eigen::Quaterniond qI( std::cos(I/2), 0.0, std::sin(I/2), 0.0 );
    const Eigen::Quaterniond qw( std::cos(omega/2), 0.0, 0.0, std::sin(omega/2) );
    const Eigen::Quaterniond Q = qW * qI * qw;
    ex = Q * ex;
    ey = Q * ey;

    // Also swapping Y and Z. It seems in Urho3D it is also left
    // ref .frame.
    km->ex.x_ = ex(0);
    km->ex.y_ = ex(2);
    km->ex.z_ = ex(1);
    km->ey.x_ = ey(0);
    km->ey.y_ = ey(2);
    km->ey.z_ = ey(1);
}

static bool genericInit( KeplerMover * km, const Eigen::Vector3d & r, const Eigen::Vector3d & v )
{
    const Float GM = km->GM;

    // First adjust vectors by 90 degrees to make it as if Oz was vertical.
    /*const Float _2 = 1.0/std::sqrt(2.0);
    const Quaterniond q( _2, _2, 0.0, 0.0 );
    const Eigen::Vector3d r = q * _r;
    const Eigen::Vector3d v = q * _v;*/

    const Float r_abs = std::sqrt( r.transpose() * r );
    const Eigen::Vector3d h = r.cross(v);
    // Need to make sure that specific angular momentum is big enough.
    // It is because if it is close to pure free radial falling equations
    // don't really work.
    {
        //const Float v_abs = std::sqrt( v.transpose() * v );
        const Float h_abs = std::sqrt( h.transpose() * h );
        const Float h_ = h_abs;
        if ( h_ < GameData::GameData::MIN_ANGULAR_MOMENTUM )
            return false;
    }


    const Eigen::Vector3d ea = v.cross( h ) / km->GM;
    const Eigen::Vector3d eb = r / r_abs;
    const Eigen::Vector3d ev = ea - eb;
    const Float e = std::sqrt( ev.transpose() * ev );
    km->e = e;

    const Eigen::Vector3d ex = ev / std::sqrt( ev.transpose() * ev );
    const Eigen::Vector3d ee = h.cross( ex );
    // If angular momentum is close to zero (moving directly towards or away)
    // second axis direction is ambiguous. Assigning it to zero in this case.
    const Float eeAbs = std::sqrt( ee.transpose() * ee );
    Eigen::Vector3d ey;
    if (eeAbs > KeplerMover::eps)
        ey = (ee / eeAbs);
    else
        ey = Eigen::Vector3d::Zero();
    km->ex.x_ = ex(0);
    km->ex.y_ = ex(1);
    km->ex.z_ = ex(2);
    km->ey.x_ = ey(0);
    km->ey.y_ = ey(1);
    km->ey.z_ = ey(2);

    if ( e > (1.0 + KeplerMover::eps) )
        hyperbolicInit( km, r, v );
    else if ( e < (1.0 -KeplerMover::eps) )
        ellipticInit( km, r, v );
    else
        parabolicInit( km, r, v );

    return true;
}

static void genericProcess( KeplerMover * km, Float t, Eigen::Vector3d & r, Eigen::Vector3d & v )
{
    const Float e = km->e;
    if ( e > (1.0 + KeplerMover::eps) )
        hyperbolicProcess( km, t, r, v );
    else if ( e < (1.0 - KeplerMover::eps) )
        ellipticProcess( km, t, r, v );
    else
        parabolicProcess( km, t, r, v );
}

static void ellipticInit( KeplerMover * km, const Eigen::Vector3d & r, const Eigen::Vector3d & v )
{
    const Float GM = km->GM;

    const Float v_2 = v.transpose() * v;
    const Float r_ = std::sqrt( r.transpose() * r );
    const Float Ws = 0.5*v_2 - GM/r_;
    const Float a = -0.5*GM/Ws; // Semimajor axis.
    km->a = a;

    // Angular momentum.
    const Eigen::Vector3d L = r.cross( v );
    const Float L_2 = L.transpose() * L;
    // Semi-latus rectum.
    const Float p = L_2 / GM;
    // Eccentricity.
    const Float e = std::sqrt( 1.0 - p/a );
    km->e = e;

    // Eccentric anomaly
    const Float cosE = (1.0 - r_/a);
    Float sinE = (r.transpose() * v);
    sinE = sinE / ( e * std::sqrt( GM * a ) );
    const Float E = std::atan2( sinE, cosE );
    km->E = E;

    // Orbital period
    const Float n = std::sqrt( a*a*a/GM );
    km->P = _2PI*n;

    // Periapsis crossing time.
    km->tau = -( E - e*std::sin(E) ) * n;
    km->timeHigh = km->tau;
    km->timeLow  = 0.0;
}

static void ellipticProcess( KeplerMover * km, Float t, Eigen::Vector3d & r, Eigen::Vector3d & v )
{
    // Solve for eccentric anomaly "E".
    const Float a = km->a;
    const Float GM = km->GM;
    const Float n = std::sqrt( (a*a*a)/GM );
    const Float M = t/n;
    Float & E = km->E;

    const Float e = km->e;
    E = ellipticSolveE( e, M, E );
    // Convert "E" to "f", "r" and "V".
    const Float coE = std::cos(E);
    const Float siE = std::sin(E);
    const Float Rx = a*(coE - e);
    const Float b = a * std::sqrt( 1 - e*e );
    const Float Ry = b*siE;

    Eigen::Vector3d ax( km->ex.x_, km->ex.y_, km->ex.z_ );
    Eigen::Vector3d ay( km->ey.x_, km->ey.y_, km->ey.z_ );
    // Position at orbit.
    r = (ax * Rx) + (ay * Ry);
}

/*static void hyperbolicInit( KeplerMover * km, Float GM, Float a, Float e, Float Omega, Float I, Float omega, Float E )
{
    km->GM = GM;
    km->a  = a;
    km->e  = e;
    km->Omega = Omega;
    km->I = I;
    km->omega = omega;
    km->E  = E;

    const Float n = std::sqrt( GM/(a*a*a) );
    const Float M = e * std::sinh(E) - E;
    t = M / n;

    km->timeHigh = t;
    km->timeLow  = 0.0;
}*/

static void hyperbolicInit( KeplerMover * km, const Eigen::Vector3d & r, const Eigen::Vector3d & v )
{
    // https://en.wikipedia.org/wiki/Hyperbolic_trajectory
    const Float GM = km->GM;

    const Float v_2 = v.transpose() * v;
    const Float r_ = std::sqrt( r.transpose() * r );
    const Float Ws = 0.5*v_2 - GM/r_;
    const Float a = -0.5*GM/Ws; // Semimajor axis.
    km->a = a;

    // Angular momentum.
    const Eigen::Vector3d L = r.cross( v );
    const Float L_2 = L.transpose() * L;
    // Semi-latus rectum.
    const Float p = L_2 / GM;
    km->l = p;

    // True anomaly.
    // r = l/(1 + e*cos(f));
    // Solve for "f" and define the sign using dot product of (r, v).
    const Float e = km->e;
    Float f = std::acos( (p/r_ - 1.0)/e );
    const Float rv = r.transpose() * v;
    const bool positive = (rv >= 0.0);
    f = positive ? f : (-f);
    km->f = f;

    // Eccentric anomaly.
    // cosh(E) = ( cos(f) + e ) / ( 1 + e * cos(f) );
    const Float coF = std::cos(f);
    Float E = std::acosh( (coF + e)/(1.0 + e*coF) );
    E = positive ? E : (-E);
    km->E = E;

    // Mean anomaly and time.
    // M = sqrt( -GM/(a*a*a) )*t = e*sinhE - E;
    const Float n = std::sqrt( -(a*a*a)/GM );
    const Float t = (e*std::sinh(E) - E) * n;
    km->timeHigh = t;
    km->tau      = t;
    km->timeLow  = 0.0;

    km->P = -1.0;
}

static void hyperbolicProcess( KeplerMover * km, Float t, Eigen::Vector3d & r, Eigen::Vector3d & v )
{
    // Solve for eccentric anomaly "E".
    const Float a = km->a;
    const Float GM = km->GM;
    const Float n = std::sqrt( -(a*a*a)/GM );
    const Float M = t/n;
    Float & E = km->E;

    const Float e = km->e;
    E = hyperbolicSolveE( e, M, E );
    // Convert "E" to "f", "r" and "V".
    const Float coE = std::cosh(E);

    // cosh(E) = (cos(f) + e)/(1 + e*cos(f))
    Float f = std::acos( (e - coE)/(e*coE - 1.0) );
    f = (E >= 0.0) ? f : (-f);
    km->f = f;

    // r = l/(1 + e*cos(f))
    const Float siF = std::sin(f);
    const Float coF = std::cos(f);
    const Float r_ = km->l / ( 1.0 + e*coF );
    const Float Rx = r_ * coF;
    const Float Ry = r_ * siF;

    // Computing real 3d coordinates.
    Eigen::Vector3d ax( km->ex.x_, km->ex.y_, km->ex.z_ );
    Eigen::Vector3d ay( km->ey.x_, km->ey.y_, km->ey.z_ );

    // Position at orbit.
    r = (ax * Rx) + (ay * Ry);
}

static Float hyperbolicNextE( const Float e, const Float M, Float & expE )
{
    // f(x) = e*sinh(x) - x - M
    // Substitute y = exp(x)
    // f(y) = e/2*(y - 1/y) - log(y) - M
    // df(y) = e/2*( 1 + 1/y^2 ) - 1/y

    //expE = expE - (0.5*e*( expE - 1.0/expE ) - std::log(expE) - M)/
    //              (0.5*e*(1.0 + 1.0/(expE*expE)) - 1.0/expE );

    expE = (2.0*expE*(e-expE+expE*std::log(expE)+expE*M)) /
           (e*expE*expE-2.0*expE+e);
    const Float err = std::abs( 0.5*e*( expE - 1.0/expE ) - std::log(expE) - M );
    return err;
}

static Float hyperbolicSolveE( const Float e, const Float M, const Float E )
{
    Float expE = std::exp(E);
    Float err = hyperbolicNextE( e, M, expE );
    int n = 0;
    while ( err >= KeplerMover::eps )
    {
        err = hyperbolicNextE( e, M, expE );

        n += 1;
        if ( n > KeplerMover::iters )
            break;
    }
    const Float En = std::log(expE);
    return En;
}

static void parabolicInit(KeplerMover * km, const Eigen::Vector3d & r, const Eigen::Vector3d & v )
{
    // https://en.wikipedia.org/wiki/Parabolic_trajectory
    const Float GM = km->GM;

    // Angular momentum.
    const Eigen::Vector3d L = r.cross( v );
    const Float L_2 = L.transpose() * L;
    // Semi-latus rectum.
    const Float p = L_2 / GM;
    km->l = p;

    const Float r_ = std::sqrt( r.transpose() * r );

    // True anomaly.
    // r = l/(1 + cos(f));
    // Solve for "f" and define the sign using dot product of (r, v).
    Float f = std::acos( (p/r_ - 1.0) );
    const Float rv = r.transpose() * v;
    const bool positive = (rv >= 0.0);
    f = positive ? f : (-f);
    km->f = f;

    // Solve for time.
    // t = sqrt( l^3/GM )*(D + D^3/3)
    // D = tan(f/2).
    const Float n = std::sqrt( (p*p*p)/GM );
    const Float D = std::tan(0.5*f);
    const Float t = 0.5*n*( D + D*D*D/3.0 );
    km->timeHigh = t;
    km->tau      = t;
    km->timeLow  = 0.0;

    km->P = -1.0;
}

static void parabolicProcess( KeplerMover * km, Float t, Eigen::Vector3d & r, Eigen::Vector3d & v )
{
    const Float GM = km->GM;
    const Float p = km->l;
    const Float rp = 0.5*p;
    const Float n = std::sqrt( GM/(2.0*rp*rp*rp) );
    const Float A = 3.0/2.0*n*t;
    const Float B = std::pow( A + std::sqrt(A*A + 1.0), 1.0/3.0 );
    const Float f = 2.0 * std::atan( B - 1.0/B );

    const Float coF = std::cos(f);
    const Float siF = std::sin(f);
    const Float r_ = p/(1.0 + coF);
    const Float Rx = r_ * coF;
    const Float Ry = r_ * siF;

    // Computing real 3d coordinates.
    Eigen::Vector3d ax( km->ex.x_, km->ex.y_, km->ex.z_ );
    Eigen::Vector3d ay( km->ey.x_, km->ey.y_, km->ey.z_ );
    // Position at orbit.
    r = (ax * Rx) + (ay * Ry);
}


}

