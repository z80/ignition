
shader_type spatial;

uniform float metallic :  hint_range(0.0, 1.0) = 0.0;
uniform float roughness : hint_range(0.0, 1.0) = 0.02;
uniform vec4  albedo :    hint_color;



void fragment()
{
	ALBEDO    = albedo.rgb;
	METALLIC  = metallic;
	ROUGHNESS = roughness;
}
