
shader_type spatial;

uniform float metallic : hint_range(0.0, 1.0) = 0.0;
uniform float roughness : hint_range(0.0, 1.0) = 0.02;

uniform int layers_qty: hint_range( 1, 4 ) = 1;

uniform sampler2D tex_albedo_0;
uniform sampler2D tex_normal_0;
uniform float     scale_0 = 1.0;
uniform float     mean_0 = 0.0;
uniform float     sigma_0 = 3.0;

uniform sampler2D tex_albedo_1;
uniform sampler2D tex_normal_1;
uniform float     scale_1 = 1.0;
uniform float     mean_1 = 0.0;
uniform float     sigma_1 = 3.0;

uniform sampler2D tex_albedo_2;
uniform sampler2D tex_normal_2;
uniform float     scale_2 = 1.0;
uniform float     mean_2 = 0.0;
uniform float     sigma_2 = 3.0;

uniform sampler2D tex_albedo_3;
uniform sampler2D tex_normal_3;
uniform float     scale_3 = 1.0;
uniform float     mean_3 = 0.0;
uniform float     sigma_3 = 3.0;


uniform sampler2D tex_albedo_slope;
uniform sampler2D tex_normal_slope;
uniform float     scale_slope = 1.0;

uniform float     threshold_slope: hint_range(0.0, 1.0) = 0.3;

//uniform sampler2D tex_displacement;
//uniform float     scale_displacement = 1.0;

// Due to floating point precision I have artifacts when texturing large 
// meshes. In order to partially leverage it I have this common point and compute 
// all colors with respect to this point.
uniform vec3 common_point;
uniform mat3 to_planet_rf;

// Total planet radius.
uniform highp float planet_radius;

vec3 compute_color( sampler2D tex, float tex_scale, highp vec2 xy, highp vec2 common_xy )
{
	highp vec2 at = xy / tex_scale + common_xy;
	vec3 color = texture( tex, at ).rgb;
	return color;
}

vec3 compute_tex_triplanar( sampler2D tex, float tex_scale, highp vec3 vertex, vec3 normal )
{
	// For some reason without common points it orks better.
	highp float common_x = mod( common_point.x / tex_scale, 1.0 );
	highp float common_y = mod( common_point.y / tex_scale, 1.0 );
	highp float common_z = mod( common_point.z / tex_scale, 1.0 );
	
	highp vec2 xy  = vertex.xy;
	vec3 color_xy  = compute_color( tex, tex_scale, xy, vec2(common_x, common_y) );
	float coeff_xy = abs(normal.z);
	
	highp vec2 xz  = vertex.xz;
	vec3 color_xz  = compute_color( tex, tex_scale, xz, vec2(common_x, common_z) );
	float coeff_xz = abs(normal.y);
	
	highp vec2 yz  = vertex.yz;
	vec3 color_yz  = compute_color( tex, tex_scale, yz, vec2(common_y, common_z) );
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
	highp float ret = den / num; //exp( -num/den );
	return ret;
}

float slope_intensity( vec3 vertex, vec3 norm )
{
	vertex += common_point;
	highp float len = length(vertex);
	vertex = vertex / len;
	float k = dot(vertex, norm);
	k = (k < threshold_slope) ? 1.0 : 0.0;
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
	normal_w = to_planet_rf * NORMAL; //( WORLD_MATRIX * vec4( NORMAL, 0.0 ) ).xyz;
	dist_w   = length(vertex_w + common_point) - planet_radius;
}


void color_1( out vec3 albedo, out vec3 normal )
{
	albedo = compute_tex_triplanar( tex_albedo_0, scale_0, vertex_w, normal_w );
	normal = compute_tex_triplanar( tex_normal_0, scale_0, vertex_w, normal_w );
}

void color_2( out vec3 albedo, out vec3 normal )
{
	vec3 albedo_0 = compute_tex_triplanar( tex_albedo_0, scale_0, vertex_w, normal_w );
	vec3 normal_0 = compute_tex_triplanar( tex_normal_0, scale_0, vertex_w, normal_w );
	float coeff_0  = tex_intensity( dist_w, mean_0, sigma_0 );
	
	vec3 albedo_1 = compute_tex_triplanar( tex_albedo_1, scale_1, vertex_w, normal_w );
	vec3 normal_1 = compute_tex_triplanar( tex_normal_1, scale_1, vertex_w, normal_w );
	float coeff_1  = tex_intensity( dist_w, mean_1, sigma_1 );
	
	float sum = coeff_0 + coeff_1;
	albedo = (albedo_0*coeff_0 + albedo_1*coeff_1) / sum;
	normal = (normal_0*coeff_0 + normal_1*coeff_1) / sum;
}

void color_3( out vec3 albedo, out vec3 normal )
{
	vec3 albedo_0 = compute_tex_triplanar( tex_albedo_0, scale_0, vertex_w, normal_w );
	vec3 normal_0 = compute_tex_triplanar( tex_normal_0, scale_0, vertex_w, normal_w );
	float coeff_0  = tex_intensity( dist_w, mean_0, sigma_0 );
	
	vec3 albedo_1 = compute_tex_triplanar( tex_albedo_1, scale_1, vertex_w, normal_w );
	vec3 normal_1 = compute_tex_triplanar( tex_normal_1, scale_1, vertex_w, normal_w );
	float coeff_1  = tex_intensity( dist_w, mean_1, sigma_1 );
	
	vec3 albedo_2 = compute_tex_triplanar( tex_albedo_2, scale_2, vertex_w, normal_w );
	vec3 normal_2 = compute_tex_triplanar( tex_normal_2, scale_2, vertex_w, normal_w );
	float coeff_2  = tex_intensity( dist_w, mean_2, sigma_2 );
	
	float sum = coeff_0 + coeff_1 + coeff_2;
	albedo = (albedo_0*coeff_0 + albedo_1*coeff_1 + albedo_2*coeff_2) / sum;
	normal = (normal_0*coeff_0 + normal_1*coeff_1 + normal_2*coeff_2) / sum;
}

void color_4( out vec3 albedo, out vec3 normal )
{
	vec3 albedo_0 = compute_tex_triplanar( tex_albedo_0, scale_0, vertex_w, normal_w );
	vec3 normal_0 = compute_tex_triplanar( tex_normal_0, scale_0, vertex_w, normal_w );
	float coeff_0  = tex_intensity( dist_w, mean_0, sigma_0 );
	
	vec3 albedo_1 = compute_tex_triplanar( tex_albedo_1, scale_1, vertex_w, normal_w );
	vec3 normal_1 = compute_tex_triplanar( tex_normal_1, scale_1, vertex_w, normal_w );
	float coeff_1  = tex_intensity( dist_w, mean_1, sigma_1 );
	
	vec3 albedo_2 = compute_tex_triplanar( tex_albedo_2, scale_2, vertex_w, normal_w );
	vec3 normal_2 = compute_tex_triplanar( tex_normal_2, scale_2, vertex_w, normal_w );
	float coeff_2  = tex_intensity( dist_w, mean_2, sigma_2 );
	
	vec3 albedo_3 = compute_tex_triplanar( tex_albedo_3, scale_3, vertex_w, normal_w );
	vec3 normal_3 = compute_tex_triplanar( tex_normal_3, scale_3, vertex_w, normal_w );
	float coeff_3  = tex_intensity( dist_w, mean_3, sigma_3 );
	
	float sum = coeff_0 + coeff_1 + coeff_2 + coeff_3;
	albedo = (albedo_0*coeff_0 + albedo_1*coeff_1 + albedo_2*coeff_2 + albedo_3*coeff_3) / sum;
	normal = (normal_0*coeff_0 + normal_1*coeff_1 + normal_2*coeff_2 + normal_3*coeff_3) / sum;
}



void fragment()
{
	vec3 albedo_c;
	vec3 normal_c;
	if ( layers_qty == 1 )
	{
		color_1( albedo_c, normal_c );
	}
	else if ( layers_qty == 2 )
	{
		color_2( albedo_c, normal_c );
	}
	else if ( layers_qty == 3 )
	{
		color_3( albedo_c, normal_c );
	}
	else
	{
		color_4( albedo_c, normal_c );
	}
	
	//float coeff_rocks = rocks_intensity( vertex_w, normal_w );
	//vec3  color_rocks = compute_tex_triplanar( tex_rocks, scale_rocks, vertex_w, normal_w );
	
	//float coeff_color = 1.0 - coeff_rocks;
	
	//color = color*coeff_color + color_rocks*coeff_rocks;
	
	ALBEDO = albedo_c;
	NORMAL = normal_c * 0.8;
	METALLIC = metallic;
	ROUGHNESS = roughness;
	//ALBEDO = mix( surface_color, depth_color_edge, 0.75 );
}
