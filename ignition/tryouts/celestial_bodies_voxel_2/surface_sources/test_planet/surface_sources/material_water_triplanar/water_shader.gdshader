
shader_type spatial;

// Water surface color.
uniform vec4 albedo_norm : source_color;
uniform vec4 albedo_tang : source_color;
uniform float metallic : hint_range(0.0, 1.0) = 0.0;
uniform float roughness : hint_range(0.0, 1.0) = 0.02;

// Waves shape.
uniform sampler2D tex_norm_a;
uniform sampler2D tex_norm_b;
uniform float tex_scale = 1.0;

// Waves dynamics.
uniform vec2 wave_velocity_a = vec2(0.2, 0.0);
uniform vec2 wave_velocity_b = vec2(0.0, 0.1);



varying vec3 vertex_w;
varying vec3 normal_w;

void vertex()
{
	vertex_w = vec3( UV.xy, UV2.x );
	normal_w = (MODEL_MATRIX * vec4( NORMAL, 0.0 )).xyz;
}

float fresnel( float amount, vec3 norm, vec3 view )
{
	float d = dot( norm, view );
	// Clamp negative dot products as zero.
	d = clamp( d, 0.0, 1.0 );
	d = 1.0 - d;
	d = pow( d, amount );
	return d;
}

vec3 surface_norm( vec2 surface_coord, vec3 normal, vec3 view )
{
	// Water surface color
	vec2 displacement_a = wave_velocity_a * TIME;
	vec2 displacement_b = wave_velocity_b * TIME;
	vec2 uv_a = surface_coord / tex_scale + displacement_a;
	vec2 uv_b = surface_coord / tex_scale + displacement_b;
	
	vec3 norm_a = texture(tex_norm_a, uv_a).rgb;
	vec3 norm_b = texture(tex_norm_b, uv_b).rgb;
	vec3 norm   = mix(norm_a, norm_b, 0.5);
	return norm;
}

void fragment()
{
	vec3 norm_xy = surface_norm( vertex_w.xy, normal_w, VIEW );
	float coeff_xy = abs(NORMAL.z);
	vec3 norm_xz = surface_norm( vertex_w.xz, normal_w, VIEW );
	float coeff_xz = abs(NORMAL.y);
	vec3 norm_yz = surface_norm( vertex_w.yz, normal_w, VIEW );
	float coeff_yz = abs(NORMAL.x);
	float sum = coeff_xy + coeff_xz + coeff_yz;
	
	vec3 norm_trilinear = NORMAL * 1.0 + (norm_xy*coeff_xy + norm_xz*coeff_xz + norm_yz*coeff_yz)/sum;
	norm_trilinear = norm_trilinear / length(norm_trilinear);

	ALBEDO = albedo_norm.rgb;
	//ALBEDO = mix( surface_color, depth_color_edge, 0.75 );
	ALPHA  = albedo_norm.a;
	METALLIC = metallic;
	ROUGHNESS = roughness;
	NORMAL    = norm_trilinear;
}
