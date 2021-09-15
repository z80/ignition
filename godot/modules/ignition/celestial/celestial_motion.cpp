
#include "celestial_motion.h"
#include "celestial_consts.h"
#include "ref_frame_node.h"
#include "math_defs.h"
#include <cmath>

#include "core/print_string.h"


using namespace Urho3D;

namespace Ign
{

CelestialMotion::CelestialMotion()
{
    type = STATIONARY;
	force_numerical = false;
    allow_orbiting = true;
 
    gm = -1.0;
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

	_debug = false;
}

CelestialMotion::~CelestialMotion()
{
}

CelestialMotion::CelestialMotion( const CelestialMotion & inst )
{
    *this = inst;
}

const CelestialMotion & CelestialMotion::operator=( const CelestialMotion & inst )
{
    if ( &inst != this )
    {
        type = inst.type;

		force_numerical = inst.force_numerical;

        allow_orbiting = inst.allow_orbiting;

        gm = inst.gm;
        h  = inst.h;
        e  = inst.e;
        abs_e = inst.abs_e;
        a = inst.a;
        A = inst.A;
        inv_A = inst.inv_A;
        slr = inst.slr;
        E = inst.E;
        n = inst.n;
        periapsis_t = inst.periapsis_t;
        T = inst.T;
        b = inst.b;
        
        se3_global = inst.se3_global;
        se3_local  = inst.se3_local;

		_debug = inst._debug;
    }

    return *this;
}

void CelestialMotion::set_allow_orbiting( bool en )
{
    allow_orbiting = en;
	// Stop orbiting if it is disabled.
	if ( !en )
		stop();
}

bool CelestialMotion::get_allow_orbiting() const
{
    return allow_orbiting;
}

void CelestialMotion::stop()
{
	if ( type != STATIONARY )
	{
		type = STATIONARY;
		se3_local.v_  = Vector3d::ZERO;
		se3_local.w_  = Vector3d::ZERO;
		se3_global.v_ = Vector3d::ZERO;
		se3_global.w_ = Vector3d::ZERO;
	}
}

bool CelestialMotion::is_orbiting() const
{
    const bool not_orbiting = (type == STATIONARY) || (gm <= 0.0);
	const bool orbiting = !not_orbiting;
	return orbiting;
}

CelestialMotion::Type CelestialMotion::movement_type() const
{
	return type;
}

Float CelestialMotion::specific_angular_momentum() const
{
	const Float h_abs = h.Length();
	return h_abs;
}

Float CelestialMotion::eccentricity() const
{
	return abs_e;
}

Float CelestialMotion::period() const
{
	if ( type != ELLIPTIC )
		return -1.0;

	const Float ret = Celestial::ticks_to_secs( T );
	return ret;
}

Float CelestialMotion::time_after_periapsis() const
{
	if ( (type != ELLIPTIC) && (type != PARABOLIC) && (type != HYPERBOLIC) )
		return 0.0;

	const Float ret = Celestial::ticks_to_secs( periapsis_t );
	return ret;
}

Float CelestialMotion::closest_approach() const
{
	if ( (type != ELLIPTIC) && (type != PARABOLIC) && (type != HYPERBOLIC) )
		return -1.0;

	const Float ret = slr / (1.0 + abs_e );
	return ret;
}

Float CelestialMotion::perigee() const
{
	if ( (type != ELLIPTIC) && (type != PARABOLIC) && (type != HYPERBOLIC) )
		return -1.0;

	const Float ret = slr / (1.0 + abs_e );
	return ret;
}

Float CelestialMotion::apogee() const
{
	if (type != ELLIPTIC)
		return -1.0;

	const Float ret = slr / (1.0 - abs_e );
	return ret;
}

Float CelestialMotion::min_velocity() const
{
	if (type != ELLIPTIC)
		return -1.0;
	const Float r = apogee();
	const Float v = std::sqrt( gm*( 2.0/r - 1.0/a ) );
	return v;
}

Float CelestialMotion::max_velocity() const
{
	if ( (type != ELLIPTIC) && (type != PARABOLIC) && (type != HYPERBOLIC) )
		return -1.0;

	const Float r = perigee();
	const Float v = std::sqrt( gm*( 2.0/r - 1.0/a ) );
	return v;
}

Float CelestialMotion::excess_velocity() const
{
	if ( (type != ELLIPTIC) && (type != PARABOLIC) && (type != HYPERBOLIC) )
		return -1.0;

	if (type == ELLIPTIC)
	{
		const Float ret = std::sqrt( gm/a );
		return ret;
	}
	if ( type == PARABOLIC )
		return 0.0;
	// HYPERBOLIC.
	const Float ret = std::sqrt( -gm/a );
	return ret;
}

Float CelestialMotion::deflection_angle() const
{
	if ( type == PARABOLIC )
		return PI1;
	if ( type != HYPERBOLIC )
		return -1.0;
	const Float ang = 2.0 * std::acos( -1.0/abs_e );
	const Float ret = ang - PI1;

	return ret;
}

Vector3d CelestialMotion::ex() const
{
	const Vector3d ret( A.m00_, A.m10_, A.m20_ );
	return ret;
}

Vector3d CelestialMotion::ey() const
{
	const Vector3d ret( A.m01_, A.m11_, A.m21_ );
	return ret;
}





void CelestialMotion::init( Float gm_, const SE3 & se3_ )
{
	_last_init_gm  = gm_;
	_last_init_se3 = se3_;

    gm  = gm_;
    se3_global = se3_;

	if ( type == ELLIPTIC )
	{
		int i = 0;
	}

	// If "gm" is not positive, shouldn't really initialize.
	if ( gm <= 0.0 )
	{
		type = STATIONARY;
		print_line( String("gm < 0.0: ") + rtos(gm) + String(", switching to idle") );
		return;
	}

    if ( !allow_orbiting )
    {
        type = STATIONARY;
		print_line( String("allow_orbiting == false, switching to idle") );
		return;
    }


    const Vector3d & r = se3_global.r_;
    const Vector3d & v = se3_global.v_;
    
    h = r.CrossProduct( v );    
    const Float abs_h = h.Length();
	const Float abs_v_x_r = r.Length() * v.Length();
	const Float abs_h_normalized = (abs_v_x_r > 0.0) ? (abs_h / abs_v_x_r) : abs_h;
	print_line( String("CelestialMotion: abs_h_normalized: ") + rtos(abs_h_normalized) );
    if ( force_numerical || (abs_h_normalized < Celestial::MIN_ANGULAR_MOMENTUM) )
    {
        type = NUMERICAL;
        init_numeric();
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
	{
        e_y = v.Normalized();
        e_y = e_y - ( e_x * e_y.DotProduct( e_x ) );
        e_y = e_y.Normalized();
	}

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

	if (_debug)
	{
		print_line( "CelestialMotion: Init" );
		print_line( "h:  " + rtos(abs_h) );
		print_line( "e:  " + rtos(abs_e) );
		print_line( "gm: " + rtos(gm) );
	}

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
	const SE3 se3_local_save = se3_local;
	const SE3 se3_global_save = se3_global;

    if (type == NUMERICAL)
        process_numeric( dt );
    else if (type == HYPERBOLIC)
        process_hyperbolic( dt );
    else if (type == ELLIPTIC)
        process_elliptic( dt );
    else if (type == PARABOLIC)
        process_parabolic( dt );

	
	//if ( isnan( se3_global.r_.x_ ) || isnan( se3_global.r_.y_ ) || isnan( se3_global.r_.z_ ) ||
	//	isnan( se3_global.v_.x_ ) || isnan( se3_global.v_.y_ ) || isnan( se3_global.v_.z_ ) )
	//{
	//	init( _last_init_gm, _last_init_se3 );

	//	//se3_local = se3_local_save;
	//	//se3_global = se3_global_save;

	//	if (type == NUMERICAL)
	//		process_numeric( dt );
	//	else if (type == HYPERBOLIC)
	//		process_hyperbolic( dt );
	//	else if (type == ELLIPTIC)
	//		process_elliptic( dt );
	//	else if (type == PARABOLIC)
	//		process_parabolic( dt );
	//}


    return se3_global;
}

const SE3 & CelestialMotion::get_se3() const
{
	return se3_global;
}


void CelestialMotion::set_se3( const SE3 & se3 )
{
	init( gm, se3 );
}


void CelestialMotion::init_numeric()
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
    const Float co_f = r.x_ / r.Length();
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


// RK4 method
// "x" - current point, "h" - integration time step.
// diff equation "dx/dt = f(x, t)".
// k1 = h*f(x, t)
// k2 = h*f(x+k1/2, t+h/2)
// k3 = h*f(x+k2/2, t+h/2)
// k4 = h*f(x+k3,   t+h)
// x = x + 1/6*(k1 + 2*k2 + 2*k3 + k4).

struct RK4_Vector6
{
	Float x[6];
};

void rk4_f( const RK4_Vector6 & x, Float gm, Float h, RK4_Vector6 & f )
{
	const Vector3d r( x.x[0], x.x[1], x.x[2] );
	const Float abs_r = r.Length();
	const Vector3d a = r * ( -gm/(abs_r*abs_r*abs_r) );
	// Position derivative is velocity.
	f.x[0] = h * x.x[3];
	f.x[1] = h * x.x[4];
	f.x[2] = h * x.x[5];
	// Velocity derivative is acceleration.
	f.x[3] = h * a.x_;
	f.x[4] = h * a.y_;
	f.x[5] = h * a.z_;
}

static void rk4_step( SE3 & se3, Float gm, Float h )
{
	RK4_Vector6 x, x1, x2, x3, k1, k2, k3, k4;
	x.x[0] = se3.r_.x_;
	x.x[1] = se3.r_.y_;
	x.x[2] = se3.r_.z_;
	x.x[3] = se3.v_.x_;
	x.x[4] = se3.v_.y_;
	x.x[5] = se3.v_.z_;

	rk4_f( x, gm, h, k1 );
	for ( int i=0; i<6; i++ )
		x1.x[i] = x.x[i] + k1.x[i]*0.5;

	rk4_f( x1, gm, h, k2 );
	for ( int i=0; i<6; i++ )
		x2.x[i] = x.x[i] + k2.x[i]*0.5;

	rk4_f( x2, gm, h, k3 );
	for ( int i=0; i<6; i++ )
		x3.x[i] = x.x[i] + k3.x[i]*0.5;

	rk4_f( x3, gm, h, k4 );

	for ( int i=0; i<6; i++ )
	{
		const Float dx_i = (k1.x[0] + 2.0*k2.x[i] + 2.0*k3.x[i] + k4.x[i]) / 6.0;
		x.x[i] += dx_i;
	}

	se3.r_.x_ = x.x[0];
	se3.r_.y_ = x.x[1];
	se3.r_.z_ = x.x[2];
	se3.v_.x_ = x.x[3];
	se3.v_.y_ = x.x[4];
	se3.v_.z_ = x.x[5];
}

void CelestialMotion::process_numeric( Float dt )
{
	//const Float abs_r = se3_global.r_.Length();
	//const Vector3d a = se3_global.r_ * ( -gm/(abs_r*abs_r*abs_r) );
	//se3_global.v_ += a * dt;
	//se3_global.r_ += se3_global.v_ * dt;

	rk4_step( se3_global, gm, dt );
	print_line( String("numerical integration: x: (" )  + rtos(se3_global.r_.x_) + String(", ") +
		rtos(se3_global.r_.y_) + String(", ") + rtos(se3_global.r_.z_) + String(")") );
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

    se3_global.r_ = this->A * se3_local.r_;
    se3_global.v_ = this->A * se3_local.v_;
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

	if (_debug)
	{

	}
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
    Float En;
    Float alpha = 1.0;
    int iters = 0;
    while (iters < Celestial::MAX_ITERS)
    {
        En = E - alpha * ( e*sih_E - E - M ) / ( e*coh_E - 1.0 );
        Float sih_En = std::sinh( En );
        err_out = std::abs( e*sih_En - En - M );
        if ( (max_err < 0.0) || (err_out < max_err) )
            break;
        iters += 1;
        alpha *= 0.5;
    }
    return En;
}





void CelestialMotion::orbit_points( RefFrameNode * own_rf, RefFrameNode * player_rf, int pts_qty, Vector<Vector3d> & pts )
{
	pts.clear();
	const SE3 rel = own_rf->relative_( player_rf );

	// r = slr/( 1 + e*cos(f) );
	for ( int i=0; i<pts_qty; i++ )
	{
		// Start at -PI in order to have points at infinity on the sides.
		const Float true_anomaly = 2.0 * M_PI * ( static_cast<Float>(i) / static_cast<Float>(pts_qty - 1) - 0.5 );
		const Float co = std::cos(true_anomaly);
		const Float den = 1.0 + abs_e*co;
		if (den < Celestial::EPS)
			continue;
		const Float si = std::sin(true_anomaly);
		const Float r = slr/den;
		const Float x = co*r;
		const Float y = si*r;
		const Vector3d local( x, y, 0.0 );
		const Vector3d r_in_own_rf = A * local;
		const Vector3d r_in_player_rf = rel.r_ + (rel.q_ * r_in_own_rf);
		pts.push_back( r_in_player_rf );
	}
}











}










