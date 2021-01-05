
#ifndef __CELECTIAL_MOTION_H_
#define __CELECTIAL_MOTION_H_

#include "se3.h"
#include "celestial_consts.h"

namespace Ign
{


class CelestialMotion
{
public:
    enum Type { LINEAR, ELLIPTIC, PARABOLIC, HYPERBOLIC };

    CelestialMotion();
    ~CelestialMotion();

    void init( Float gm, const SE3 & se3 );
    static Float init_gm( Float radius_km, Float wanted_surface_orbit_velocity_kms );
    void launch_elliptic( Float gm, const Vector3d & unit_r, const Vector3d & unit_v, Float period_hrs, Float eccentricity );
    const SE3 & process( Float dt );

    Type type;

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
    void init_parabolic();
    void init_elliptic();
    void init_hyperbolic();

    void process_linear( Float dt );
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




