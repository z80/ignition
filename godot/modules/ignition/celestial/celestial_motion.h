
#ifndef __CELECTIAL_MOTION_H_
#define __CELECTIAL_MOTION_H_

#include "se3.h"
#include "celestial_consts.h"

namespace Ign
{


class CelestialMotion
{
public:
    enum Type { STATIONARY=0, LINEAR=1, ELLIPTIC=2, PARABOLIC=3, HYPERBOLIC=4 };

    CelestialMotion();
    ~CelestialMotion();

    CelestialMotion( const CelestialMotion & inst );
    const CelestialMotion & operator=( const CelestialMotion & inst );

    void set_allow_orbiting( bool en );
    bool get_allow_orbiting() const;

    void set_stationary_threshold( Float th );
    Float get_stationary_threshold() const;

    void stop();

	// Functionality for requesting state.
    bool is_orbiting() const;
	Type movement_type() const;
	Float specific_angular_momentum() const;
	Float eccentricity() const;
	Float period() const;
	Float time_after_periapsis() const;
	Float closest_approach() const;
	Float perigee() const;
	Float apogee() const;
	Float min_velocity() const;
	Float max_velocity() const;
	Float excess_velocity() const;
	Float deflection_angle() const;
	// Orbit orientation.
	// From focus towards perigee.
	Vector3d ex() const;
	// Along velocity at perigee.
	Vector3d ey() const;


	// Functionality needed for processing.
    void init( Float gm, const SE3 & se3 );
    static Float init_gm( Float radius_km, Float wanted_surface_orbit_velocity_kms );
    void launch_elliptic( Float gm, const Vector3d & unit_r, const Vector3d & unit_v, Float period_hrs, Float eccentricity );
    const SE3 & process( Float dt );

    Type type;

    Float stationary_threshold;
    bool allow_orbiting;

    Float    gm;
    Vector3d h;
    Vector3d e;
    Float    abs_e;
    Float    a;
    Matrix3d A;
    Matrix3d inv_A;
    Float    slr;
    Float    E;
    Float    n;
    Celestial::Ticks periapsis_t;
    Celestial::Ticks T;
    Float    b;
    // These two are needed in linear motion.
    // But "r" and "v" already exist in SE3 data type. So exclude 
    // "r" and "v" from needed state fields.
    //Vector3d r;
    //Vector3d v;
    SE3 se3_global;
    SE3 se3_local;

private:
    void init_linear();
    void init_linear_gravity();
    void init_parabolic();
    void init_elliptic();
    void init_hyperbolic();

    void process_linear( Float dt );
    void process_linear_gravity( Float dt );
    void process_parabolic( Float dt );
    void process_elliptic( Float dt );
    void process_hyperbolic( Float dt );

    Vector3d velocity_parabolic() const;
    Vector3d velocity_elliptic() const;
    Vector3d velocity_hyperbolic() const;

    static Float solve_elliptic( Float e, Float M, Float E );
    static Float solve_next_elliptic( Float e, Float M, Float E, Float max_err, Float & err_out );

    static Float solve_hyperbolic( Float e, Float M, Float E );
    static Float solve_next_hyperbolic( Float e, Float M, Float E, Float max_err, Float & err_out );

};


}



#endif




