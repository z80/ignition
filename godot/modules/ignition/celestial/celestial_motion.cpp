
#include "celestial_motion.h"
#include "celestial_consts.h"
#include <cmath>

namespace Ign
{

CelestialMotion::CelestialMotion()
{
    type = LINEAR;

    gm = 1.0;
    abs_e = 0.0;
    a = 1.0;
    A = Matrix3d::IDENTITY;
    inv_A = Matrix3d::IDENTITY;
    slr = 1.0;
    E = 0.0;
    n = 1.0;
    periapsis_t = 0;
    T = 1000;
    b = 1.0;
}

CelestialMotion::~CelestialMotion()
{
}


void CelestialMotion::init( Float gm_, const SE3 & se3_ )
{
    gm  = gm_;
    se3_global = se3_;
    const Vector3d & r = se3_global.r_;
    const Vector3d & v = se3_global.v_;
    
    h = r.CrossProduct( v );    
    const Float abs_h = h.Length();
    if ( abs_h < Celestial::MIN_ANGULAR_MOMENTUM )
    {
        type = LINEAR;
        init_linear();
        return;
    }

    const Float abs_r = r.Length();
    const Vector3d & ea = v.CrossProduct( h ) / gm;
    const Vector3d & eb = r / abs_r;
    // Eccentricity vector.
    e     = ea - eb;
    abs_e = e.Length();

    const Float abs_v = v.Length();
    const Float Ws    = gm/abs_r - 0.5*abs_v*abs_v;
    // Semimajor axis.
    if ( ( abs_e > (1.0 + Celestial::EPS) ) || 
         ( abs_e < (1.0 - Celestial::EPS) ) )
        a = 0.5*gm/Ws;
    else
        a = 0.0;

    // Orbit plane unit vectors.
    // "e_a" towards perigee.
    // "e_b" 90 degrees from perigee along speed vector.
    Vector3d e_x;
    if (abs_e > Celestial::EPS)
        e_x = e / abs_e;
    else
        e_x = r.Normalized();

    const Vector3d h_cross_ex = h.CrossProduct( e_x );
    const Float abs_h_cross_ex = h_cross_ex.Length();
    Vector3d e_y;
    if ( abs_h_cross_ex > Celestial::EPS )
        e_y = h_cross_ex / abs_h_cross_ex;
    else
        e_y = v.Normalized();
        e_y = e_y - ( e_x * e_y.DotProduct( e_x ) );
        e_y = e_y.Normalized();

    const Vector3d e_z = e_x.CrossProduct( e_y );
    A.m00_ = e_x.x_;
    A.m10_ = e_x.y_;
    A.m20_ = e_x.z_;

    A.m01_ = e_y.x_;
    A.m11_ = e_y.y_;
    A.m21_ = e_y.z_;

    A.m02_ = e_z.x_;
    A.m12_ = e_z.y_;
    A.m22_ = e_z.z_;

    inv_A = A.Transpose();

    se3_local.r_ = inv_A * se3_global.r_;
    se3_local.v_ = inv_A * se3_global.v_;

    if ( abs_e > (1.0 + Celestial::EPS) )
    {
        type = HYPERBOLIC;
        init_hyperbolic();
    }
    else if ( abs_e < (1.0 - Celestial::EPS) )
    {
        type = ELLIPTIC;
        init_elliptic();
    }
    else
    {
        type = PARABOLIC;
        init_parabolic();
    }
}

Float CelestialMotion::init_gm( Float radius_km, Float wanted_surface_orbit_velocity_kms )
{
    const Float v = wanted_surface_orbit_velocity_kms * 1000.0;
    const Float r = radius_km * 1000.0;
    const Float gm = v*v*r;
    return gm;
}

void CelestialMotion::launch_elliptic( Float gm, const Vector3d & unit_r, const Vector3d & unit_v, Float period_hrs, Float eccentricity )
{
    const Float ee = (1.0 + eccentricity) / (1.0 - eccentricity);
    const Float period = period_hrs * 3600.0;
    const Float pi_T = (PI2 * gm) / period;
    const Float arg = pi_T*pi_T*ee*ee*ee;
    const Float v = std::pow( arg, 1.0/6.0 );
    const Float r = ((1.0+eccentricity)*gm) / (v*v);

    Vector3d v_r = unit_r;
    v_r.Normalize();
    Vector3d v_v = unit_v;
    v_v.Normalize();
    v_v = v_v - v_r * v_r.DotProduct(v_v);
    v_v.Normalize();

    v_r = v_r * r;
    v_v = v_v * v;

	SE3 se3;
	se3.r_ = v_r;
	se3.v_ = v_v;
    init( gm, se3 );
}

const SE3 & CelestialMotion::process( Float dt )
{
    if (type == LINEAR)
        process_linear( dt );
    else if (type == HYPERBOLIC)
        process_hyperbolic( dt );
    else if (type == ELLIPTIC)
        process_elliptic( dt );
    else if (type == PARABOLIC)
        process_parabolic( dt );

    return se3_global;
}

void CelestialMotion::init_linear()
{
    // Nothing here.
}

void CelestialMotion::init_parabolic()
{
    // Semi latus rectum.
    const Float abs_h = h.Length();
    slr = (abs_h * abs_h) / gm;

    // True anomaly.
    const Vector3d & r = se3_local.r_;
    const Float abs_r = r.Length();
    Float arg = slr/abs_r - 1.0;
    if (arg > 1.0)
        arg = 1.0;
    else if (arg < -1.0)
        arg = -1.0;
    Float true_anomaly = std::acos(arg);
    const bool neg = (r.y_ < 0.0);
    if ( neg )
        true_anomaly = -true_anomaly;

    const Float D = std::tan( true_anomaly * 0.5 );
    const Float t = 0.6 * std::sqrt( (slr*slr*slr)/gm ) * ( D + (D*D*D)/3.0 );
    periapsis_t = Celestial::secs_to_ticks( t );
}

void CelestialMotion::init_elliptic()
{
    const Vector3d & r = se3_local.r_;
    const Vector3d & v = se3_local.v_;

    // Semi latus rectum.
    slr = a * (1.0 - abs_e*abs_e);

    b = a * std::sqrt(1.0 - abs_e*abs_e);
    const Float cos_E = (r.x_ + a*abs_e) / a;
    const Float sin_E = r.y_ / b;
    
    E = std::atan2( sin_E, cos_E );

    n = std::sqrt( (a*a*a)/gm );

    // Orbital period.
    const Float pT = PI2*n;
    T = Celestial::secs_to_ticks( pT );
    // Time since periapsis.
    const Float pt = n * ( E - abs_e*std::sin(E) );
    periapsis_t = Celestial::secs_to_ticks( pt );

}

void CelestialMotion::init_hyperbolic()
{
    // Semi latus rectum.
    slr = a * (1.0 - abs_e*abs_e);
    n   = std::sqrt( -(a*a*a)/gm );

    // True anomaly from "r".
    const Vector3d & r = se3_local.r_;
    const Float co_f = r.x_;
    const Float cosh_E = (co_f + abs_e)/(1.0 + abs_e*co_f);
    Float sinh_E = std::sqrt(cosh_E*cosh_E - 1.0); 
    if ( r.y_ < 0.0 )
        sinh_E = -sinh_E;
    const Float exp_E = cosh_E + sinh_E;
    const Float E = std::log( exp_E );

    const Float M = abs_e * sinh_E - E;
    const Float pt = M * n;

    periapsis_t = Celestial::secs_to_ticks( pt );
}

void CelestialMotion::process_linear( Float dt )
{
    const Float abs_r = se3_global.r_.Length();
    const Vector3d a = se3_global.r_ * ( -gm/(abs_r*abs_r*abs_r) );
    se3_global.v_ += a * dt;
    se3_global.r_ += se3_global.v_ * dt;
}

void CelestialMotion::process_parabolic( Float dt )
{
    const Celestial::Ticks d_ticks = Celestial::secs_to_ticks( dt );
    periapsis_t += d_ticks;

    const Float pt = Celestial::ticks_to_secs( periapsis_t );
    const Float rp = slr * 0.5;
    const Float A = 1.5 * std::sqrt( gm/(2.0*rp*rp*rp) ) * pt;
    const Float B = std::pow( A + std::sqrt(A*A + 1.0), 1.0/3.0 );
    const Float true_anomaly = 2.0*std::atan( B - 1.0/B );

    const Float co_f = std::cos( true_anomaly );
    const Float si_f = std::sin( true_anomaly );

    const Float abs_r = slr/(1.0 + co_f);

    const Float r_x = abs_r * co_f;
    const Float r_y = abs_r * si_f;

    se3_local.r_ = Vector3d( r_x, r_y, 0.0 );
    se3_local.v_ = velocity_parabolic();

    se3_global.r_ = A * se3_local.r_;
    se3_global.v_ = A * se3_local.v_;
}

void CelestialMotion::process_elliptic( Float dt )
{
    const Celestial::Ticks d_ticks = Celestial::secs_to_ticks( dt );
    periapsis_t += d_ticks;
    if (periapsis_t > T)
        periapsis_t -= T;

    const Float pt = Celestial::ticks_to_secs( periapsis_t );

    const Float M = pt / n;
    E = solve_elliptic( abs_e, M, E );

    const Float co_E = std::cos(E);
    const Float si_E = std::sin(E);
    const Float x = a*(co_E - abs_e);
    const Float y = b*si_E;
    se3_local.r_ = Vector3d( x, y, 0.0 );
    se3_local.v_ = velocity_elliptic();

    se3_global.r_ = A * se3_local.r_;
    se3_global.v_ = A * se3_local.v_;
}

void CelestialMotion::process_hyperbolic( Float dt )
{
    const Float d_ticks = Celestial::secs_to_ticks( dt );
    periapsis_t += d_ticks;
    const Float pt = Celestial::ticks_to_secs( periapsis_t );
    
    const Float M = pt / n;
    E = solve_hyperbolic( abs_e, M, E );

    const Float coh_E = std::cosh( E );
    const Float co_f = (coh_E - abs_e)/(1.0 - abs_e*coh_E);
    Float si_f = std::sqrt( 1.0 - co_f*co_f );
    if (E < 0.0)
        si_f = -si_f;

    const Float abs_r = slr/(1.0 + abs_e*co_f);
    const Float x = abs_r * co_f;
    const Float y = abs_r * si_f;

    se3_local.r_ = Vector3d( x, y, 0.0 );
    se3_local.v_ = velocity_hyperbolic();

    se3_global.r_ = A * se3_local.r_;
    se3_global.v_ = A * se3_local.v_;
}

Vector3d CelestialMotion::velocity_parabolic() const
{
    const Float abs_r = se3_local.r_.Length();

    const Float si = se3_local.r_.y_ / abs_r;
    const Float co = se3_local.r_.x_ / abs_r;

    Vector3d v( -si, co + 1.0, 0.0 );
    v.Normalize();
    const Float abs_v = std::sqrt( (2.0*gm)/abs_r );
    v = v * abs_v;

    return v;
}

Vector3d CelestialMotion::velocity_elliptic() const
{
    const Float abs_r = se3_local.r_.Length();
    Vector3d v( -a*std::sin(E), b*std::cos(E), 0.0 );
    v.Normalize();
    const Float abs_v = std::sqrt( gm*( 2.0/abs_r - 1.0/a ) );
    v *= abs_v;

    return v;
}

Vector3d CelestialMotion::velocity_hyperbolic() const
{
	const Vector3d & r = se3_local.r_;
    const Float abs_r = r.Length();

    const Float si_f = r.y_ / abs_r;
    const Float co_f = r.x_ / abs_r;

    Vector3d v( -si_f, co_f + abs_e, 0.0 );
    v.Normalize();
    const Float abs_v = std::sqrt( gm*( 2.0/abs_r - 1.0/a ) );
    v = v * abs_v;

    return v;
}


Float CelestialMotion::solve_elliptic( Float e, Float M, Float E )
{
    Float err;
    Float En = solve_next_elliptic( e, M, E, -1.0, err );

    int iters = 0;
    while ( (err > Celestial::EPS) && (iters < Celestial::MAX_ITERS) )
    {
        En = solve_next_elliptic( e, M, En, err, err );
        iters += 1;
    }
    if ( En > PI1 )
        En -= PI2;
    else if ( En < -PI1 )
        En += PI2;

    return En;
}

Float CelestialMotion::solve_next_elliptic( Float e, Float M, Float E, Float max_err, Float & err_out )
{
    const Float si_E = std::sin( E );
    const Float co_E = std::cos( E );
    Float En = E;
    Float alpha = 1.0;
    int iters = 0;
    while ( iters < Celestial::MAX_ITERS )
    {
        En = E - alpha*(E - e*si_E - M) / (1.0 - e*co_E);
        const Float si_En = std::sin(En);
        err_out = std::abs(En - e*si_En - M);
        if ( (max_err < 0.0) || (err_out < max_err) )
            break;
        alpha = alpha * 0.5;
        iters += 1;
    }

    return En;
}


Float CelestialMotion::solve_hyperbolic( Float e, Float M, Float E )
{
    Float err;
    Float En = solve_next_hyperbolic( e, M, E, -1.0, err );
    int iters = 0;

    while ( (err > Celestial::EPS) && (iters < Celestial::MAX_ITERS) )
    {
        En = solve_next_hyperbolic( e, M, En, err, err );
        iters += 1;
    }

    return En;
}

Float CelestialMotion::solve_next_hyperbolic( Float e, Float M, Float E, Float max_err, Float & err_out )
{
    const Float sih_E = std::sinh( E );
    const Float coh_E = std::cosh( E );
    Float err;
    Float En;
    Float alpha = 1.0;
    int iters = 0;
    while (iters < Celestial::MAX_ITERS)
    {
        En = E - alpha * ( e*sih_E - E - M ) / ( e*coh_E - 1.0 );
        Float sih_En = std::sinh( En );
        err = std::abs( e*sih_En - En - M );
        if ( (max_err < 0.0) || (err < max_err) )
            break;
        iters += 1;
        alpha *= 0.5;
    }
    return En;
}














}










