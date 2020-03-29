
#define NOMINMAX

#include "deterministic_source_0.h"
#include "pi_terrain_noise.h"
#include "pi_consts.h"
#include "Urho3D/Urho3DAll.h"

namespace Ign
{

#define COUNTOF( arr ) (sizeof(arr)/sizeof(arr[0]))


DeterministicSource0::DeterministicSource0( const PiSourceDesc & body )
    : DeterministicSource( body )
{
    setFracDef( 0, m_maxHeightInMeters * 0.05, 1e6, 10000.0 );
    const Float height = m_maxHeightInMeters * 0.3;
    setFracDef( 1, height, m_rand.Double( 4.0, 20.0 ) * height );
}

DeterministicSource0::~DeterministicSource0()
{
}

Float DeterministicSource0::height( const Vector3d & at ) const
{
    Float n = octavenoise( getFracDef(0), 0.4, at ) * dunes_octavenoise( getFracDef(1), 0.5, at );
    n *= 10.0;

    return (n > 0.0 ? m_maxHeight * n : 0.0);
}

Color DeterministicSource0::color( const Vector3d & at, const Vector3d & norm, Float height )  const
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


