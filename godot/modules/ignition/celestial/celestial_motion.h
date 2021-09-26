
#ifndef __CELECTIAL_MOTION_H_
#define __CELECTIAL_MOTION_H_

#include "se3.h"
#include "celestial_consts.h"

#include "core/math/transform.h"

namespace Ign
{

class RefFrameNode;

class CelestialMotion
{
public:
    enum Type { STATIONARY=0, NUMERICAL=1, ELLIPTIC=2, PARABOLIC=3, HYPERBOLIC=4 };

    CelestialMotion();
    ~CelestialMotion();

    CelestialMotion( const CelestialMotion & inst );
    const CelestialMotion & operator=( const CelestialMotion & inst );

    void set_allow_orbiting( bool en );
    bool get_allow_orbiting() const;

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

    const SE3 & get_se3() const;
    void set_se3( const SE3 & se3 );

	// For drawing orbits.
	void orbit_points( RefFrameNode * orbiting_center_node, RefFrameNode * player_viewpoint_node, int pts_qty, Vector<Vector3d> & pts );


    Type type;
	bool force_numerical;

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

	bool _debug;
	SE3 _last_init_se3;
	Float _last_init_gm;

private:
    void init_numeric();
    void init_parabolic();
    void init_elliptic();
    void init_hyperbolic();

    void process_numeric( Float dt );
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


	// For placing manuver nodes.
	//static Float eccentric_from_true_anomaly_elliptic( Float f );
};


}



#endif




