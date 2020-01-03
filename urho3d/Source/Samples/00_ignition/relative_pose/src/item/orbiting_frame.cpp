
#include "orbiting_frame.h"

namespace Ign
{


static const Float MIN_ANGULAR_MOMENTUM = 0.1;
static const Float EPS  = 0.001;
static const int   ITERS = 32;

static Float parentGM( RefFrame * rf );

static bool initGeneric( OrbitingFrame * of, const Vector3d & r, const Vector3d & v );
static void initHyperbolic( OrbitingFrame * of, const Vector3d & r, const Vector3d & v );
static void initParabolic( OrbitingFrame * of, const Vector3d & r, const Vector3d & v );
static void initElliptic( OrbitingFrame * of, const Vector3d & r, const Vector3d & v );

static void processGeneric( OrbitingFrame * of, Timestamp t );
static void processHyperbolic( OrbitingFrame * of, Timestamp t );
static void processParabolic( OrbitingFrame * of, Timestamp t );
static void processElliptic( OrbitingFrame * of, Timestamp t );

static Float hyperbolicNextE( const Float e, const Float M, Float & expE )
static Float hyperbolicSolveE( const Float e, const Float M, const Float E )

static Float ellipticNextE( const Float e, const Float M, Float & E );
static Float ellipticSolveE( const Float e, const Float M, const Float E );
    

void OrbitingFrame::RegisterObject( Context * context )
{
}

OrbitingFrame::OrbitingFrame( Context * context )
    : EvolvingFrame( context )
{
}

OrbitingFrame::~OrbitingFrame()
{
}

void OrbitingFrame::evolveStep( Timestamp ticks_dt )
{
    orbitDesc.periapsisTime += ticks_dt;
    processGeneric( this, orbitDesc.periapsisTime );
}

bool OrbitingFrame::Recursive() const
{
    return false;
}

bool OrbitingFrame::ProducesForces() const
{
    return true;
}

void OrbitingFrame::ComputeForces( PhysicsItem * receiver, const State & st, Vector3d & F, Vector3d & P ) const
{
    const Float r_abs = st.r.Length();
    F = (-GM/(r_abs*r_abs*r_abs))*st.r;
    P = Vector3d::ZERO;
}

void OrbitingFrame::SetGM( Float GM )
{
    GM_ = GM;
}

Float OrbitingFrame::GM() const
{
    return GM_;
}

void OrbitingFrame::SetR( Float R )
{
    R_ = R;
}

Float OrbitingFrame::R() const
{
    return R_;
}

void OrbitingFrame::Launch( const Vector3d & r, const Vector3d & v )
{
    initGeneric( this, r, v );
}











static Float parentGM( RefFrame * rf )
{
    RefFrame * p = rf->parent();
    if ( !rf )
        return -1.0;
    OrbitingFrame * of = p->Cast<OrbitingFrame>();
    if ( of )
        return of->GM();
    const Float gm = parentGM( p );
    return gm;
}

static bool initGeneric( OrbitingFrame * of, const Vector3d & r, const Vector3d & v )
{
    const Float gm = parentGM( this );
    if ( gm <= 0.0 )
        return false;

    of->orbitDesc.gm = gm;

    // Compute specific angular momentum
    const Vector3d h = r.CrossProduct( v );
    // Need to make sure that specific angular momentum is big enough.
    // It is because if it is close to pure free radial falling equations
    // don't really work.
    {
        const Float h_abs = h.Length();
        if ( h_abs < MIN_ANGULAR_MOMENTUM )
            return false;
    }


    const Float r_abs = r.Length();


    const Vector3d ea = v.CrossProduct( h ) / gm;
    const Vector3d eb = r / r_abs;
    // "ecc" - vector eccentricity.
    const Vector3d ecc = ea - eb;
    const Float ecc_abs = ecc.Length();
    of->orbitDesc.eccentricity = ecc_abs;

    // Semimajor axis
    const Float v_abs = v.Length();
    const Float Ws = 0.5*v_abs*v_abs - gm/r_abs;
    const Float a = -0.5*gm/Ws; // Semimajor axis.
    of->orbitDesc.semimajorAxis = a;

    // Vector eccentricity direction vector.
    const Vector3d ex = ecc / ecc_abs;
    const Vector3d ee = h.cross( ex );
    // If angular momentum is close to zero (moving directly towards or away)
    // second axis direction is ambiguous. Assigning it to zero 
    // in this case.
    const Float ee_abs = ee.Length();
    Vector3d ey;
    if ( ee_abs > EPS )
        ey = ee / ee_abs;
    else
        ey = Vector3d::ZERO;
    of->orbitDesc.ex = ex;
    of->orbitDesc.ey = ey;

    if ( ecc_abs > (1.0 + EPS) )
        initHyperbolic( of, r, v );
    else if ( ecc_abs < (1.0 - EPS) )
        initElliptic( of, r, v );
    else
        initParabolic( of, r, v );
}


/* For hyperbolic I need 
 * 0) semimajor axis
 * 1) eccentricity absolute value
 * 2) eccentric anomaly
 * 3) gm
 */

static void initHyperbolic( OrbitingFrame * of, Float gm, const Vector3d & r, const Vector3d & v )
{
    // https://en.wikipedia.org/wiki/Hyperbolic_trajectory
    const Float gm = of->orbitDesc.gm;

    // Specific angular momentum.
    const Vector3d h = r.CrossProduct( v );
    // Semi-latus rectum.
    const Float p = h.DotProduct( h ) / gm;

    // True anomaly.
    // r = l/(1 + e*cos(f));
    // Solve for "f" and define the sign using dot product of (r, v).
    const Float ecc = of->orbitDesc.eccentricity;
    Float f = std::acos( (p/r_abs - 1.0)/ecc );
    const Float rv = r.DotProduct( v );
    const bool positive = (rv >= 0.0);
    f = positive ? f : (-f);

    // Eccentric anomaly.
    // cosh(E) = ( cos(f) + e ) / ( 1 + e * cos(f) );
    const Float coF = std::cos(f);
    Float E = std::acosh( (coF + e)/(1.0 + e*coF) );
    E = positive ? E : (-E);
    of->orbitDesc.eccentricAnomaly = E;

    // Periapsis crossing time.
    const Float n = std::sqrt( -(a*a*a)/gm );
    const Float t = (e*std::sinh(E) - E) * n;
    of->orbitDesc.periapsisTime = static_cast<Timestamp>( t * static_cast<Float>( Settings::ticksPerSec() ) );
}

static void initParabolic( OrbitingFrame * of, Float gm, const Vector3d & r, const Vector3d & v )
{
    // https://en.wikipedia.org/wiki/Parabolic_trajectory
    const Float gm = of->orbitDesc.gm;

    // Angular momentum.
    const Vector3d h = r.CrossProduct( v );
    // Semi-latus rectum.
    const Float p = h.DotProduct( h ) / gm;
    of->orbitDesc.semiLatusRectum = p;
}

static void initElliptic( OrbitingFrame * of, Float gm, const Vector3d & r, const Vector3d & v )
{
    const Float gm = of->orbitDesc.gm;
    const Float a  = of->orbitDesc.semimajorAxis;
    const Float ecc_abs = of->orbitDesc.eccentricity;
    const Float r_abs = r.Length();

    // Eccentric anomaly
    const Float cosE = (1.0 - r_abs/a);
    Float sinE = r.DotPRoduct( v );
    sinE = sinE / ( ecc_abs * std::sqrt( gm * a ) );
    const Float E = std::atan2( sinE, cosE );
    of->orbitDesc.eccentricAnomaly = E;

    // Orbital period
    const Float n = std::sqrt( a*a*a/gm );
    const Float T = PI2*n;
    of->orbitDesc.period = static_cast<Timestamp>( T * static_cast<Float>( Settings::ticksPerSec() ) );

    // Periapsis crossing time.
    const Float t = -( E - e*std::sin(E) ) * n;
    of->orbitDesc.periapsisTime = static_cast<Timestamp>( t * static_cast<Float>( Settings::ticksPerSec() ) );
}
 











static void processGeneric( OrbitingFrame * of, Timestamp t )
{
    Vector3d r, v;
    const Float ecc = of->orbitDesc.eccentricity;
    if ( ecc > (1.0 + EPS) )
        processHyperbolic( of, t, r, v );
    else if ( ecc < (1.0 - EPS) )
        processElliptic( of, t, r, v );
    else
        processParabolic( of, t, r, v );
}

static void processHyperbolic( OrbitingFrame * of, Timestamp t, Vector3d & r, Vector3d & v )
{
    // Solve for eccentric anomaly "E".
    const Float a = of->orbitDesc.semimajorAxis;
    const Float gm = of->orbitDesc.gm;
    const Float n = std::sqrt( -(a*a*a)/gm );
    const Float M = t/n;
    Float & E = of->orbitDesc.eccentricAnomaly;

    const Float ecc = of->orbitDesc.eccentricity;
    E = hyperbolicSolveE( ecc, M, E );
    // Convert "E" to "f", "r" and "V".
    const Float coE = std::cosh(E);

    // cosh(E) = (cos(f) + e)/(1 + e*cos(f))
    Float f = std::acos( (e - coE)/(e*coE - 1.0) );
    f = (E >= 0.0) ? f : (-f);

    // r = l/(1 + e*cos(f))
    const Float siF = std::sin(f);
    const Float coF = std::cos(f);
    const Float p = of->orbitDesc.semiLatusRectum;
    const Float r_ = p / ( 1.0 + e*coF );
    const Float Rx = r_ * coF;
    const Float Ry = r_ * siF;

    // Computing real 3d coordinates.
    const Vector3d & ex = of->orbitDesc.ex;
    const Vector3d & ey = of->orbitDesc.ey;

    // Position at orbit.
    r = (ex * Rx) + (ey * Ry);

    // Periapsis time.
    const Float t = (e*std::sinh(E) - E) * n;
    of->orbitDesc.periapsisTime = static_cast<Timestamp>( t * static_cast<Float>( Settings::ticksPerSec() ) );
}

static void processParabolic( OrbitingFrame * of, Timestamp t )
{
    const Float gm = of->orbitDesc.gm;
    const Float p  = of->orbitDesc.semiLatusRectum;
    const Float rp = 0.5*p;
    const Float n = std::sqrt( gm/(2.0*rp*rp*rp) );
    const Float A = 3.0/2.0*n*t;
    const Float B = std::pow( A + std::sqrt(A*A + 1.0), 1.0/3.0 );
    const Float f = 2.0 * std::atan( B - 1.0/B );

    const Float coF = std::cos(f);
    const Float siF = std::sin(f);
    const Float r_ = p/(1.0 + coF);
    const Float Rx = r_ * coF;
    const Float Ry = r_ * siF;

    // Computing real 3d coordinates.
    const Vector3d & ex = of->orbitDesc.ex;
    const Vector3d & ey = of->orbitDesc.ey;
 
    // Position at orbit.
    r = (ex * Rx) + (ey * Ry);

    // Need to check this "nn" and declared before "n". Those are supposed to be 
    // the same thing ???
    const Float nn = std::sqrt( (p*p*p)/GM );
    const Float D = std::tan(0.5*f);
    const Float t = 0.5*nn*( D + D*D*D/3.0 );
    of->orbitDesc.periapsisTime = static_cast<Timestamp>( t * static_cast<Float>( Settings::ticksPerSec() ) );
}

static void processElliptic( OrbitingFrame * of, Timestamp t )
{
    // Solve for eccentric anomaly "E".
    const Float a  = of->orbitDesc.semimajorAxis;
    const Float gm = of->orbitDesc.gm;
    const Float n = std::sqrt( (a*a*a)/gm );
    const Float M = t/n;
    Float & E = of->orbitDesc.eccentricAnomaly;

    const Float e = km->e;
    E = ellipticSolveE( e, M, E );
    // Convert "E" to "f", "r" and "V".
    const Float coE = std::cos(E);
    const Float siE = std::sin(E);
    const Float Rx = a*(coE - e);
    const Float b = a * std::sqrt( 1.0 - e*e );
    const Float Ry = b*siE;

    const Vector3d & ex = of->orbitDesc.ex;
    const Vector3d & ey = of->orbitDesc.ey;
  
    // Position at orbit.
    r = (ex * Rx) + (ey * Ry);

    // Periapsis crossing time.
    const Float t = -( E - e*std::sin(E) ) * n;
    of->orbitDesc.periapsisTime = static_cast<Timestamp>( t * static_cast<Float>( Settings::ticksPerSec() ) );
} 










static Float hyperbolicNextE( const Float e, const Float M, Float & expE )
{
    const Float nextExpE = (2.0*expE*(e-expE+expE*std::log(expE)+expE*M)) /
                           (e*expE*expE-2.0*expE+e);
    expE = nextExpE;
    const Float err = std::abs( 0.5*e*( expE - 1.0/expE ) - std::log(expE) - M );
    return err;
}

static Float hyperbolicSolveE( const Float e, const Float M, const Float E )
{
    Float expE = std::exp(E);
    Float err = hyperbolicNextE( e, M, expE );
    int n = 0;
    while ( err >= EPS )
    {
        err = hyperbolicNextE( e, M, expE );

        n += 1;
        if ( n > ITERS )
            break;
    }
    const Float En = std::log(expE);
    return En;
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
    while ( err >= EPS )
    {
        err = ellipticNextE( e, M, En );

        n += 1;
        if ( n > ITERS )
            break;
    }
    if ( En > PI2 )
        En -= PI2;
    else if ( En < 0.0 )
        En += PI2;
    return En;
}

static Float speed( const Float gm, const Float a, const Float r, bool parabolic )
{
    if ( !parabolic )
    {
        const Float v = std::sqrt( gm*( 2.0/r - 1.0/a ) );
        return v;
    }
    const Float v = std::sqrt( 2.0*gm/r );
    return v;
}

static void velocity( OrbitingFrame * of, Float & vx, Float & vy, bool parabolic )
{
    /*
    // Hyperbolic
    {
        const Float E = of->orbitDesc.eccentricAnomaly;
        const Float coE = std::cosh(E);
        Float f = std::acos( (e - coE)/(e*coE - 1.0) );
        f = (E >= 0.0) ? f : (-f);
    }
    // Elliptic
    {

    }

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
    */
}







}







