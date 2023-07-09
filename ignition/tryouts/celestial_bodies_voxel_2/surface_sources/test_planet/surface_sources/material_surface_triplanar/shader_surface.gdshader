
shader_type spatial;

const int LAYERS_QTY = 4;

uniform float metallic : hint_range(0.0, 1.0) = 0.0;
uniform float roughness : hint_range(0.0, 1.0) = 0.02;

uniform int layers_qty: hint_range( 1, 4 ) = 1;

uniform sampler2D [LAYERS_QTY] tex_albedo;
//uniform sampler2D [LAYERS_QTY] tex_normal;
uniform float     [LAYERS_QTY] scale;
uniform float     [LAYERS_QTY] mean;
uniform float     [LAYERS_QTY] sigma;

uniform sampler2D tex_albedo_slope;
//uniform sampler2D tex_normal_slope;
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

float tex_intensity( float dist_, float mean_, float sigma_ )
{
	//highp float num = abs(dist_ - mean_) + 1.0;
	highp float num = abs(dist_ - mean_);
	highp float den = sigma_;
	//highp float ret = den / num; //exp( -num/den );
	highp float ret = exp( -num/den );
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
	normal_w = to_planet_rf * NORMAL; //( MODEL_MATRIX * vec4( NORMAL, 0.0 ) ).xyz;
	dist_w   = length(vertex_w + common_point) - planet_radius;
}




void compute_combined_color( out vec3 albedo )
{
	float accum = 0.0;
	albedo = vec3(0.0, 0.0, 0.0);
	//normal = vec3(0.0, 0.0, 0.0);
	for ( int i=0; i<layers_qty; i++ )
	{
		vec3 albedo_ = compute_tex_triplanar( tex_albedo[i], scale[i], vertex_w, normal_w );
		//vec3 normal_ = compute_tex_triplanar( tex_normal[i], scale[i], vertex_w, normal_w );
		float coeff_  = tex_intensity( dist_w, mean[i], sigma[i] );
		accum += coeff_;
		albedo += albedo_ * coeff_;
		//normal += normal_ * coeff_;
	}
//	vec3 albedo_ = compute_tex_triplanar( tex_albedo[0], scale[0], vertex_w, normal_w );
//	vec3 normal_ = compute_tex_triplanar( tex_normal[0], scale[0], vertex_w, normal_w );
//	float coeff_  = tex_intensity( dist_w, mean[0], sigma[0] );
//
//	albedo = albedo_;
//	normal = normal_;

	accum = 1.0 / (accum + 1.0e-6);
	albedo *= accum;
	//normal *= accum;
}





void fragment()
{
	vec3 albedo_c;
	//vec3 normal_c;
	compute_combined_color( albedo_c );
	
	//float coeff_rocks = rocks_intensity( vertex_w, normal_w );
	//vec3  color_rocks = compute_tex_triplanar( tex_rocks, scale_rocks, vertex_w, normal_w );
	
	//float coeff_color = 1.0 - coeff_rocks;
	
	//color = color*coeff_color + color_rocks*coeff_rocks;
	
	ALBEDO    = albedo_c;
	//NORMAL    = normal_c * 0.8;
	METALLIC  = metallic;
	ROUGHNESS = roughness;
	//ALBEDO = mix( surface_color, depth_color_edge, 0.75 );
}
