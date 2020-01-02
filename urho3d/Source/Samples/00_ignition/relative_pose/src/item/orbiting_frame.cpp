
#include "orbiting_frame.h"

namespace Ign
{


static const Float MIN_ANGULAR_MOMENTUM = 0.1;
static const Float EPS = 0.001;

static Float parentGM( RefFrame * rf );

static bool initGeneric( OrbitingFrame * of, const Vector3d & r, const Vector3d & v );
static void initHyperbolic( OrbitingFrame * of, const Vector3d & r, const Vector3d & v );
static void initParabolic( OrbitingFrame * of, const Vector3d & r, const Vector3d & v );
static void initElliptic( OrbitingFrame * of, const Vector3d & r, const Vector3d & v );

static void processGeneric( OrbitingFrame * of, Timestamp t );
static void processHyperbolic( OrbitingFrame * of, Timestamp t );
static void processParabolic( OrbitingFrame * of, Timestamp t );
static void processElliptic( OrbitingFrame * of, Timestamp t );
    

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
}

void OrbitingFrame::SetGM( Float GM )
{
}

Float OrbitingFrame::GM() const
{
    return GM_;
}

void OrbitingFrame::SetR( Float R )
{
}

Float OrbitingFrame::R() const
{
    return R_;
}

void OrbitingFrame::Launch( const Vector3d & r, const Vector3d & v )
{
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
}

static void processHyperbolic( OrbitingFrame * of, Timestamp t )
{
}

static void processParabolic( OrbitingFrame * of, Timestamp t )
{
}

static void processElliptic( OrbitingFrame * of, Timestamp t )
{
} 




}







