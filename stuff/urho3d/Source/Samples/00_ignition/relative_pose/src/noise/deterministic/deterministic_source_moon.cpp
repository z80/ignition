
#define NOMINMAX

#include "deterministic_source_moon.h"
#include "pi_terrain_noise.h"
#include "pi_terrain_feature.h"
#include "pi_consts.h"
#include "Urho3D/Urho3DAll.h"

namespace Ign
{

#define COUNTOF( arr ) (sizeof(arr)/sizeof(arr[0]))


DeterministicSourceMoon::DeterministicSourceMoon( const PiSourceDesc & body )
    : DeterministicSource( body )
{    
    setFracDef( 0, m_maxHeightInMeters * 0.001, m_planetRadius * 5.0, m_planetRadius * 0.02 );
    const Float height = m_maxHeightInMeters * 0.03;
    setFracDef( 1, height, m_rand.Double( 4.0, 20.0 ) * height );

    setFracDef( 2, m_maxHeightInMeters * 0.02, m_planetRadius * 0.7, m_planetRadius * 0.01 );
    setFracDef( 3, m_maxHeightInMeters * 0.01, m_planetRadius * 0.2, m_planetRadius * 0.01 );
}

DeterministicSourceMoon::~DeterministicSourceMoon()
{
}

Float DeterministicSourceMoon::height( const Vector3d & at ) const
{
    Float n = octavenoise( getFracDef(0), 0.4, at );
    //n *= dunes_octavenoise( getFracDef(1), 0.5, at );
    //n *= 5.0;

    n += crater_function( getFracDef(2), at );
    n += crater_function( getFracDef(3), at );

    return (n > 0.0 ? m_maxHeight * n : 0.0);
}

Color DeterministicSourceMoon::color( const Vector3d & at, const Vector3d & norm, Float height )  const
{
    Float n = m_invMaxHeight * height / 2;

    Vector3d col;
    if (n <= 0.02)
    {
        const Float flatness = pow(at.DotProduct(norm), 6.0);
        const Vector3d color_cliffs = m_rockColor[1];

        Float equatorial_desert = (2.0) * (-1.0 + 2.0 * octavenoise(12, 0.5, 2.0, (n * 2.0) * at)) *
            1.0 * (2.0) * (1.0 - at.y_ * at.y_);

        col = interpolate_color(equatorial_desert, m_rockColor[0], m_greyrockColor[3]);
        col = interpolate_color(n, col, Vector3d(1.5, 1.35, 1.3));
        col = interpolate_color(flatness, color_cliffs, col);
    }
    else
    {
        const Float flatness = pow(at.DotProduct(norm), 6.0);
        const Vector3d color_cliffs = m_greyrockColor[1];

        Float equatorial_desert = (2.0) * (-1.0 + 2.0 * octavenoise(12, 0.5, 2.0, (n * 2.0) * at)) *
            1.0 * (2.0) * (1.0 - at.y_ * at.y_);

        col = interpolate_color(equatorial_desert, m_greyrockColor[0], m_greyrockColor[2]);
        col = interpolate_color(n, col, m_rockColor[3]);
        col = interpolate_color(flatness, color_cliffs, col);
    }

    const Color c( col.x_, col.y_, col.z_ );
    return c;
}


}


