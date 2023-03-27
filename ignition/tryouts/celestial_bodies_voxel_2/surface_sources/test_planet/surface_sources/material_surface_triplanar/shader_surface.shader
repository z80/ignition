
shader_type spatial;

uniform float metallic : hint_range(0.0, 1.0) = 0.0;
uniform float roughness : hint_range(0.0, 1.0) = 0.02;

uniform sampler2D tex_sand;
uniform float     scale_sand = 1.0;
uniform float     mean_sand = 0.0;
uniform float     sigma_sand = 3.0;

uniform sampler2D tex_grass;
uniform float     scale_grass = 1.0;
uniform float     mean_grass = 50.0;
uniform float     sigma_grass = 300.0;

uniform sampler2D tex_snow;
uniform float     scale_snow = 1.0;
uniform float     mean_snow = 50.0;
uniform float     sigma_snow = 3.0;

uniform sampler2D tex_rocks;
uniform float     scale_rocks = 1.0;

uniform float     threshold_rocks: hint_range(0.0, 1.0) = 0.3;

uniform sampler2D tex_displacement;
uniform float     scale_displacement = 1.0;

// Rotational transform for converting normals from 
// the local ref. frame on the surface to planet center re. frame.
uniform mat3 mat_to_planet_center;

// Total planet radius.
uniform highp float planet_radius;

vec3 compute_color( sampler2D tex, float tex_scale, highp vec2 xy )
{
	highp vec2 at = xy / tex_scale;
	vec3 color = texture( tex, at ).rgb;
	return color;
}

vec3 compute_tex_triplanar( sampler2D tex, float tex_scale, highp vec3 vertex, vec3 normal )
{
	highp vec2 xy       = vertex.xy;
	vec3 color_xy = compute_color( tex, tex_scale, xy );
	float coeff_xy = abs(normal.z);
	
	highp vec2 xz = vertex.xz;
	vec3 color_xz = compute_color( tex, tex_scale, xz );
	float coeff_xz = abs(normal.y);
	
	highp vec2 yz = vertex.yz;
	vec3 color_yz = compute_color( tex, tex_scale, yz );
	float coeff_yz = abs(normal.x);
	
	float sum = coeff_xy + coeff_xz + coeff_yz;
	
	vec3 color = (color_xy*coeff_xy + color_xz*coeff_xz + color_yz*coeff_yz) / sum;
	return color;
}

float tex_intensity( float dist, float mean, float sigma )
{
	highp float num = dist - mean;
	num = num * num;
	highp float den = sigma * sigma;
	highp float ret = exp( -num/den );
	return ret;
}

float rocks_intensity( vec3 vertex, vec3 norm )
{
	highp float len = length(vertex);
	vertex = vertex / len;
	float k = dot(vertex, norm);
	k = (k < threshold_rocks) ? 1.0 : 0.0;
	return k;
}


varying highp vec3  vertex_w;
varying highp vec3  normal_w;
varying highp float dist_w;

void vertex()
{
	vertex_w = vec3( UV.xy, UV2.x );
	//vertex_w = ( WORLD_MATRIX * vec4( VERTEX, 1.0 ) ).xyz;
	//normal_w = mat_to_planet_center * NORMAL;
	normal_w = ( WORLD_MATRIX * vec4( NORMAL, 0.0 ) ).xyz;
	dist_w   = length(vertex_w) - planet_radius;
}



void fragment()
{
	vec3  color_sand = compute_tex_triplanar( tex_sand, scale_sand, vertex_w, normal_w );
	float coeff_sand = tex_intensity( dist_w, mean_sand, sigma_sand );

	vec3  color_grass = compute_tex_triplanar( tex_grass, scale_grass, vertex_w, normal_w );
	float coeff_grass = tex_intensity( dist_w, mean_grass, sigma_grass );

	vec3  color_snow = compute_tex_triplanar( tex_snow, scale_snow, vertex_w, normal_w );
	float coeff_snow = tex_intensity( dist_w, mean_snow, sigma_snow );
	
	float sum  = coeff_sand + coeff_grass + coeff_snow;
	vec3 color = (color_sand*coeff_sand + color_grass*coeff_grass + color_snow*coeff_snow) / sum;
	
	float coeff_rocks = rocks_intensity( vertex_w, normal_w );
	vec3  color_rocks = compute_tex_triplanar( tex_rocks, scale_rocks, vertex_w, normal_w );
	
	float coeff_color = 1.0 - coeff_rocks;
	
	//color = color*coeff_color + color_rocks*coeff_rocks;
	
	ALBEDO = color;
	METALLIC = metallic;
	ROUGHNESS = roughness;
//	//ALBEDO = mix( surface_color, depth_color_edge, 0.75 );
//	ALPHA  = albedo_norm.a;
//	NORMAL    = norm_trilinear;
}
