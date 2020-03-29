
#define NOMINMAX

#include "deterministic_source_0.h"
#include "pi_terrain_noise.h"
#include "pi_consts.h"
#include "Urho3D/Urho3DAll.h"

namespace Ign
{
/*static  fixed Clamp( fixed v, Float vmin, Float vmax )
{
    fixed r;
    if ( v < vmin )
        r = vmin;
    else if ( v > vmax )
        r = vmax;
    else
        r = v;
    return r;
}*/

#define COUNTOF( arr ) (sizeof(arr)/sizeof(arr[0]))


DeterministicSource0::DeterministicSource0( const PiSourceDesc & body )
    : HeightSource(), 
    m_seed(body.seed_),
    m_rand(body.seed_),
    m_minh(0)
{
    src_ = body;

    m_heightScaling = 1.0;

    m_sealevel = Clamp(body.liquid_.ToDouble(), 0.0, 1.0);
    m_icyness = Clamp(body.ice_.ToDouble(), 0.0, 1.0);
    m_volcanic = Clamp(body.volcanic_.ToDouble(), 0.0, 1.0); // height scales with volcanicity as well
    m_surfaceEffects = 0;

    // Radius in meters.
    const Float rad = src_.radius();
    // mass in kilograms.
    const Float mass = body.mass();
    // calculate max height
    const Float empiricHeight = (9000.0 * rad * rad * (m_volcanic + 0.5)) / ( mass * 6.64e-12 );
    // max mountain height for earth-like planet (same mass, radius)
    m_maxHeightInMeters = std::max(100.0, empiricHeight );
    m_maxHeightInMeters = std::min(rad, m_maxHeightInMeters); // small asteroid case

                                                              // and then in sphere normalized jizz
    m_maxHeight = m_maxHeightInMeters / rad;
    //Output("%s: max terrain height: %fm [%f]\n", m_minBody.name.c_str(), m_maxHeightInMeters, m_maxHeight);
    m_invMaxHeight = 1.0 / m_maxHeight;
    m_planetRadius = rad;
    m_invPlanetRadius = 1.0 / rad;
    m_planetEarthRadii = rad / EARTH_RADIUS;

    // Pick some colors, mainly reds and greens
    for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
        m_entropy[i] = m_rand.Double();
    for (int i = 0; i < int(COUNTOF(m_rockColor)); i++)
    {
        double r, g, b;
        r = m_rand.Double(0.3, 1.0);
        g = m_rand.Double(0.3, r);
        b = m_rand.Double(0.3, g);
        r = std::max(b, r * body.metal_.ToDouble());
        g = std::max(b, g * body.metal_.ToDouble());
        m_rockColor[i] = Vector3d(r, g, b);
    }

    // Pick some darker colours mainly reds and greens
    for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
        m_entropy[i] = m_rand.Double();
    for (int i = 0; i < int(COUNTOF(m_darkrockColor)); i++)
    {
        Float r, g, b;
        r = m_rand.Double(0.05, 0.3);
        g = m_rand.Double(0.05, r);
        b = m_rand.Double(0.05, g);
        r = std::max(b, r * body.metal_.ToDouble());
        g = std::max(b, g * body.metal_.ToDouble());
        m_darkrockColor[i] = Vector3d(r, g, b);
    }

    // grey colours, in case you simply must have a grey colour on a world with high metallicity
    for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
        m_entropy[i] = m_rand.Double();
    for (int i = 0; i < int(COUNTOF(m_greyrockColor)); i++)
    {
        Float g;
        g = m_rand.Double(0.3, 0.9);
        m_greyrockColor[i] = Vector3d(g, g, g);
    }

    // Pick some plant colours, mainly greens
    // TODO take star class into account
    for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
        m_entropy[i] = m_rand.Double();
    for (int i = 0; i < int(COUNTOF(m_plantColor)); i++)
    {
        Float r, g, b;
        g = m_rand.Double(0.3, 1.0);
        r = m_rand.Double(0.3, g);
        b = m_rand.Double(0.2, r);
        g = std::max(r, g * body.life_.ToDouble());
        b *= (1.0 - body.life_.ToDouble());
        m_plantColor[i] = Vector3d(r, g, b);
    }

    // Pick some darker plant colours mainly greens
    // TODO take star class into account
    for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
        m_entropy[i] = m_rand.Double();
    for (int i = 0; i < int(COUNTOF(m_darkplantColor)); i++)
    {
        Float r, g, b;
        g = m_rand.Double(0.05, 0.3);
        r = m_rand.Double(0.00, g);
        b = m_rand.Double(0.00, r);
        g = std::max(r, g * body.life_.ToDouble());
        b *= (1.0 - body.life_.ToDouble());
        m_darkplantColor[i] = Vector3d(r, g, b);
    }

    // Pick some sand colours, mainly yellow
    // TODO let some planetary value scale this colour
    for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
        m_entropy[i] = m_rand.Double();
    for (int i = 0; i < int(COUNTOF(m_sandColor)); i++)
    {
        Float r, g, b;
        r = m_rand.Double(0.6, 1.0);
        g = m_rand.Double(0.6, r);
        //b = m_rand.Double(0.0, g/2.0);
        b = 0;
        m_sandColor[i] = Vector3d(r, g, b);
    }

    // Pick some darker sand colours mainly yellow
    // TODO let some planetary value scale this colour
    for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
        m_entropy[i] = m_rand.Double();
    for (int i = 0; i < int(COUNTOF(m_darksandColor)); i++)
    {
        Float r, g, b;
        r = m_rand.Double(0.05, 0.6);
        g = m_rand.Double(0.00, r);
        //b = m_rand.Double(0.00, g/2.0);
        b = 0;
        m_darksandColor[i] = Vector3d(r, g, b);
    }

    // Pick some dirt colours, mainly red/brown
    // TODO let some planetary value scale this colour
    for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
        m_entropy[i] = m_rand.Double();
    for (int i = 0; i < int(COUNTOF(m_dirtColor)); i++)
    {
        Float r, g, b;
        r = m_rand.Double(0.3, 0.7);
        g = m_rand.Double(r - 0.1, 0.75);
        b = m_rand.Double(0.0, r / 2.0);
        m_dirtColor[i] = Vector3d(r, g, b);
    }

    // Pick some darker dirt colours mainly red/brown
    // TODO let some planetary value scale this colour
    for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
        m_entropy[i] = m_rand.Double();
    for (int i = 0; i < int(COUNTOF(m_darkdirtColor)); i++)
    {
        double r, g, b;
        r = m_rand.Double(0.05, 0.3);
        g = m_rand.Double(r - 0.05, 0.35);
        b = m_rand.Double(0.0, r / 2.0);
        m_darkdirtColor[i] = Vector3d(r, g, b);
    }

    // These are used for gas giant colours, they are more m_random and *should* really use volatileGasses - TODO
    for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
        m_entropy[i] = m_rand.Double();
    for (int i = 0; i < int(COUNTOF(m_gglightColor)); i++)
    {
        double r, g, b;
        r = m_rand.Double(0.0, 0.5);
        g = m_rand.Double(0.0, 0.5);
        b = m_rand.Double(0.0, 0.5);
        m_gglightColor[i] = Vector3d(r, g, b);
    }
    //darker gas giant colours, more reds and greens
    for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
        m_entropy[i] = m_rand.Double();
    for (int i = 0; i < int(COUNTOF(m_ggdarkColor)); i++)
    {
        double r, g, b;
        r = m_rand.Double(0.0, 0.3);
        g = m_rand.Double(0.0, r);
        b = m_rand.Double(0.0, std::min(r, g));
        m_ggdarkColor[i] = Vector3d(r, g, b);
    }


    setFracDef( 0, m_maxHeightInMeters * 0.05, 1e6, 10000.0 );
    const Float height = m_maxHeightInMeters * 0.3;
    setFracDef( 1, height, m_rand.Double( 4.0, 20.0 ) * height );
}

DeterministicSource0::~DeterministicSource0()
{
}

Float DeterministicSource0::height( const Vector3d & at ) const
{
    const Float n = octavenoise( getFracDef(0), 0.4, at ) * dunes_octavenoise( getFracDef(1), 0.5, at );

    return (n > 0.0 ? m_maxHeight * n : 0.0);
}

Color DeterministicSource0::color( const Vector3d & at )  const
{
    /*
    Float n = m_invMaxHeight * height / 2;

    if (n <= 0.02)
    {
        const Float flatness = pow(p.DotProduct(norm), 6.0);
        const Vector3d color_cliffs = m_rockColor[1];

        Float equatorial_desert = (2.0) * (-1.0 + 2.0 * octavenoise(12, 0.5, 2.0, (n * 2.0) * p)) *
            1.0 * (2.0) * (1.0 - p.y_ * p.y_);

        Vector3d col;
        col = interpolate_color(equatorial_desert, m_rockColor[0], m_greyrockColor[3]);
        col = interpolate_color(n, col, Vector3d(1.5, 1.35, 1.3));
        col = interpolate_color(flatness, color_cliffs, col);
        return col;
    }
    else
    {
        const Float flatness = pow(p.DotProduct(norm), 6.0);
        const Vector3d color_cliffs = m_greyrockColor[1];

        Float equatorial_desert = (2.0) * (-1.0 + 2.0 * octavenoise(12, 0.5, 2.0, (n * 2.0) * p)) *
            1.0 * (2.0) * (1.0 - p.y_ * p.y_);

        Vector3d col;
        col = interpolate_color(equatorial_desert, m_greyrockColor[0], m_greyrockColor[2]);
        col = interpolate_color(n, col, m_rockColor[3]);
        col = interpolate_color(flatness, color_cliffs, col);
        return col;
    }*/

    return Color();
}

void DeterministicSource0::setFracDef( const unsigned int index, const Float featureHeightMeters, const Float featureWidthMeters, const Float smallestOctaveMeters )
{
    assert(index >= 0 && index < MAX_FRACDEFS);
    // feature
    m_fracdef[index].amplitude = featureHeightMeters / (m_maxHeight * m_planetRadius);
    m_fracdef[index].frequency = m_planetRadius / featureWidthMeters;
    m_fracdef[index].octaves = std::max(1, int(ceil(log(featureWidthMeters / smallestOctaveMeters) / log(2.0))));
    m_fracdef[index].lacunarity = 2.0;
    //Output("%d octaves\n", m_fracdef[index].octaves); //print
}

}


