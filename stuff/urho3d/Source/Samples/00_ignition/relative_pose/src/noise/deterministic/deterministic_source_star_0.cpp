
#define NOMINMAX

#include "deterministic_source_star_0.h"
#include "pi_terrain_noise.h"
#include "pi_terrain_feature.h"
#include "pi_consts.h"
#include "Urho3D/Urho3DAll.h"

namespace Ign
{

#define COUNTOF( arr ) (sizeof(arr)/sizeof(arr[0]))


DeterministicSourceStar0::DeterministicSourceStar0( const PiSourceDesc & body )
    : DeterministicSource( body )
{
    src_.super_type_ = SUPERTYPE_STAR;

    // Color related stuff
    Float height = m_maxHeightInMeters * 0.1;
    setFracDef( 0, height, 0.8*m_planetRadius, 0.01 * m_planetRadius );
    setFracDef( 1, height, 0.7*m_planetRadius, 0.1 * m_planetRadius );
    setFracDef( 2, height, 0.4*m_planetRadius, 0.001 * m_planetRadius );
    setFracDef( 3, height, 0.02*m_planetRadius, 0.001 * m_planetRadius );
}

DeterministicSourceStar0::~DeterministicSourceStar0()
{
}

Float DeterministicSourceStar0::height( const Vector3d & at ) const
{
    const Float ar = src_.aspectRatio_.ToDouble();
    // x_^2 = (p.z_^2+p.x_^2) (eqn. 5)
    const Float x_squared = (at.x_ * at.x_ + at.z_ * at.z_);
    // y_ = p.y_
    const Float y_squared = at.y_ * at.y_;
    const Float distFromCenter_R = ar / sqrt(x_squared + ar * ar * y_squared); // (eqn. 9)
                                                                               // GetHeight must return the difference in the distance from center between a point in a sphere of
                                                                               // Polar radius (in coords scaled to a unit sphere) and the point on the ellipsoid surface.
    return std::max(distFromCenter_R - 1.0, 0.0);
}

Color DeterministicSourceStar0::color( const Vector3d & at, const Vector3d & norm, Float height )  const
{
    Float n;
    Vector3d col;
    n = octavenoise(getFracDef(0), 0.5, at) * 0.5;
    n += voronoiscam_octavenoise( getFracDef(1), 0.5, at ) * 0.5;
    n += octavenoise( getFracDef(0), 0.5, at ) * billow_octavenoise( getFracDef(1), 0.5, at );
    n += octavenoise( getFracDef(2), 0.5, at ) * 0.5 * Clamp( getFracDef(0).amplitude - 0.2, 0.0, 1.0 );
    n += 15.0 * billow_octavenoise( getFracDef(0), 0.8, piSimplex( at * 3.142) * at ) *
        megavolcano_function( getFracDef(1), at );
    n *= n * 0.15;
    n = 1.0 - n;
    if (n > 0.666)
    {
        //n -= 0.666; n *= 3.0;
        //col = interpolate_color(n, Vector3d(1.0, 1.0, 1.0), Vector3d(1.0, 1.0, 1.0) );
        col = Vector3d(1.0, 1.0, 1.0);
    }
    else if (n > 0.333)
    {
        n -= 0.333;
        n *= 3.0;
        col = interpolate_color(n, Vector3d(.6, .6, .0), Vector3d(1.0, 1.0, 1.0));
    }
    else if (n > 0.05)
    {
        n -= 0.05;
        n *= 3.533;
        col = interpolate_color(n, Vector3d(.8, .8, .0), Vector3d(.6, .6, .0));
    }
    else
    {
        n *= 20.0;
        col = interpolate_color(n, Vector3d(.02, .0, .0), Vector3d(.8, .8, .0));
    }

    const Color c( col.x_, col.y_, col.z_ );
    return c;
}


}


