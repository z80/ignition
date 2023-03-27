shader_type spatial;

// Water surface color.
uniform vec4 albedo_norm : hint_color;
uniform vec4 albedo_tang : hint_color;
uniform float metallic : hint_range(0.0, 1.0) = 0.0;
uniform float roughness : hint_range(0.0, 1.0) = 0.02;

// Waves shape.
uniform sampler2D tex_norm_a;
uniform sampler2D tex_norm_b;
uniform float tex_scale = 1.0;

// Waves dynamics.
uniform vec2 wave_velocity_a = vec2(0.2, 0.0);
uniform vec2 wave_velocity_b = vec2(0.0, 0.1);

// Objects color under the surface.
uniform vec4 color_deep : hint_color;
uniform vec4 color_shallow: hint_color;
uniform float beers_law = 2.0;
uniform float depth_offset = -0.75;

// Water edge.
uniform float edge_scale = 0.1;
uniform float near = 1.0;
uniform float far  = 100.0;

uniform vec4 edge_color: hint_color;

varying vec3 vert;

void vertex()
{
	vert = (WORLD_MATRIX * vec4( VERTEX, 1.0 )).xyz;
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

float edge( float depth )
{
	float d = 2.0 * depth - 1.0;
	d = (near * far) / ( far + depth * (near - far) );
	return d;
}

float edge_( sampler2D depth_texture, vec2 screen_uv, vec3 fragcoord )
{
	float z_depth = edge( texture(depth_texture, screen_uv).x );
	float z_pos = edge( fragcoord.z );
	float z_diff = z_depth - z_pos;
	float ret = step( edge_scale, z_diff );
	return ret;
}

vec3 underwater_color( sampler2D depth_texture, sampler2D screen_texture, vec2 screen_uv, mat4 projection_matrix, vec3 vertex )
{
	// Objects under water.
	float depth = texture(depth_texture, screen_uv).r * 2.0 - 1.0;
	depth = projection_matrix[3][2] / (depth + projection_matrix[2][2]);
	depth = exp( -(depth + vertex.z + depth_offset) * beers_law );
	depth = clamp( pow(depth, 2.5), 0.0, 1.0 );
	
	
	vec3 screen_color = textureLod( screen_texture, screen_uv, depth * 2.5 ).rgb;
	vec3 depth_color  = mix( color_shallow.rgb, color_deep.rgb, depth );
	vec3 ret = mix(screen_color * depth_color, depth_color * 0.25, depth * 0.5);
	return ret;
}

void fragment()
{
	// Underwater color.
	vec3 under_color = underwater_color( DEPTH_TEXTURE, SCREEN_TEXTURE, SCREEN_UV, PROJECTION_MATRIX, VERTEX );
	
	// Water edge coefficient.
	//float edge_coeff = edge_( DEPTH_TEXTURE, SCREEN_UV, FRAGCOORD.xyz );
	// Mixing 
	//vec3 depth_color_edge = mix( edge_color.rgb, under_color, edge_coeff );

	// Water surface color
	vec2 displacement_a = wave_velocity_a * TIME;
	vec2 displacement_b = wave_velocity_b * TIME;
	vec2 uv_a = vert.xz / tex_scale + displacement_a;
	vec2 uv_b = vert.xz / tex_scale + displacement_b;
	
	vec3 norm_a = texture(tex_norm_a, uv_a).rgb * 1.0;
	vec3 norm_b = texture(tex_norm_b, uv_b).rgb * 1.0;
	vec3 norm   = NORMAL + mix(norm_a, norm_b, 0.5);
	norm = norm / length(norm);
	
	float fresnel_mag = fresnel( 5.0, NORMAL, VIEW );
	vec3 surface_color = mix( albedo_norm.rgb, albedo_tang.rgb, fresnel_mag );
	
	ALBEDO = mix( surface_color, under_color, 0.75 );
	//ALBEDO = mix( surface_color, depth_color_edge, 0.75 );
	ALPHA  = albedo_norm.a;
	METALLIC = metallic;
	ROUGHNESS = roughness;
	NORMAL    = norm;
}
