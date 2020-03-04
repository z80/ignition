
#include "atmosphere_source.h"
#include "physics_item.h"
#include "air_mesh.h"

namespace Ign
{

AtmosphereSource::AtmosphereSource()
{
    normalCoefP_ = 0.1;
    normalCoefN_ = 0.2;
    lateralCoef_ = 0.02;
}

AtmosphereSource::~AtmosphereSource()
{
}

bool AtmosphereSource::params( Float distFromCenter, Float & pressure, Float & density ) const
{
    pressure = 0.0;
    density  = 0.0;

    return false;
}

bool AtmosphereSource::drag( AirMesh & a, const State & st, Vector3d & F, Vector3d & P ) const
{
    const Float dist = st.r.Length();
    Float pressure;
    Float density;
    const bool inAtmosphere = params( dist, pressure, density );
    if ( !inAtmosphere )
        return false;

    const Quaterniond Q = st.q;
    const Quaterniond invQ = st.q.Inverse();

    // Velocity in block ref. frame.
    const Vector3d V0 = invQ*st.v;
    // Angular velocity in block ref. frame.
    const Vector3d W = invQ*st.w;

    // Need to redeclare all these in PhysicsItem.
    Vector<ForceApplied> & friction = a.forces_;
    // Compute forces in block ref. frame.
    const unsigned qty = a.triangles.Size();
    friction.Reserve( qty );

    for ( unsigned i=0; i<qty; i++ )
    {
        const Triangle & t = a.triangles[i];
        // Velocity at triangle center.
        //const Vector3d V = V0 + W.CrossProduct( t.at );
        const Vector3d V = V0;
        const Float absV = V.Length();
        const Float V_n = V.DotProduct( t.n );
        ForceApplied fa;

        // Dynamic pressure force.
        const Float normalCoeff = ( V_n >= 0.0 ) ? normalCoefP_ : normalCoefN_;
        const Float abs_V_n = std::abs( V_n );
        const Float A = t.a;
        const Vector3d F_normal_local = -(normalCoeff*abs_V_n*absV*absV*density*A) * t.n;
        const Vector3d F_normal = Q * F_normal_local;

        // Viscosity force.
        const Vector3d lateralV = V - t.n*V_n;
        const Vector3d F_lateral_local = -(A*lateralCoef_*density)*lateralV;
        const Vector3d F_lateral = Q * F_lateral_local;

        fa.Fn  = F_normal;
        fa.Fl  = F_lateral;
        const Vector3d m = (t.v[0] + t.v[1] + t.v[2]) * 0.33333;
        fa.at = Q*m + st.r;
        friction.Push( fa );

        F += F_normal;
        F += F_lateral;

        const Vector3d P_normal_local  = t.at.CrossProduct( F_normal_local );
        const Vector3d P_normal = Q * P_normal_local;
        const Vector3d P_lateral_local = t.at.CrossProduct( F_lateral_local );
        const Vector3d P_lateral = Q * P_lateral_local;
        P += P_normal;
        P += P_lateral;
    }

    return true;
}


}





