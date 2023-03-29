shader_type spatial;
render_mode unshaded;

// Atmosphere parameters
uniform vec4  color_day:   hint_color = vec4( 0.65, 0.8, 1.0, 1.0 );
uniform vec4  color_night: hint_color = vec4( 1.0, 0.43, 0.46, 1.0 );
uniform float planet_radius = 1000.0;
uniform float height  = 10.0;
uniform float transparency_distance_inner = 1.0;
uniform float transparency_distance_outer = 1000.0;
// From camera towards the light in camera ref. frame.
uniform vec3  light_dir     = vec3(0.0, 1.0, 0.0);
// Planet center in camera ref. frame.
uniform vec3  planet_center = vec3(0.0, -950.0, -433.0);
uniform float displacement: hint_range(-5.0, 5.0);


// Returns "true" if intersection length is nonzero.
// Otherwise returns "false".
bool ray_atmosphere_intersection_length( vec3 ray, float sphere_radius, out float out_length, out float out_closest_distance, out bool out_inside )
{
	float origin_distance_2    = dot( planet_center, planet_center );
	float d_to_closest_point   = dot( planet_center, ray );
	float d_to_closest_point_2 = d_to_closest_point * d_to_closest_point;
	// Pithagorous theorem.
	float closest_distance_2   = origin_distance_2 - d_to_closest_point_2;
	float sphere_radius_2 = sphere_radius * sphere_radius;
	if ( closest_distance_2 > sphere_radius_2 )
	{
		// Ray doesn't go through.
		out_inside = false;
		out_length = -1.0;
		return false;
	}
	
	// Compute distance from closest point to sphere intersection points.
	// Also use Pithagorous theorem for this purpose.
	float intersect_dist_2 = sphere_radius_2 - closest_distance_2;
	
	float closest_dist   = sqrt( closest_distance_2 );
	float intersect_dist = sqrt( intersect_dist_2 );
	float distance_positive = intersect_dist;
	
	bool is_in_front = ( (d_to_closest_point + intersect_dist) > 0.0 );
	if ( !is_in_front )
	{
		// Ray doesn't go through.
		out_inside = false;
		out_length = -1.0;
		return false;
	}
	
	float distance_negative;
	// Return "inside".
	out_inside = ( d_to_closest_point_2 < intersect_dist_2 );
	if ( out_inside )
		distance_negative = closest_dist;
	else
		distance_negative = distance_positive;
	
	// Returning intersection length "len".
	out_length = distance_positive + distance_negative;
	
	// Returning "day_evening". It is "1.0" for day time and "0.0" for night time.
	//out_day_night = dot( ray/(length(ray)+0.001), light_dir/(length(light_dir) + 0.001) ); //clamp( (dot( ray/length(ray), light_dir/length(light_dir) ) + 1.0)*0.5 + 0.0, 0.0, 1.0 );
	
	// Return the closest distance from the line of sight to the planet center.
	out_closest_distance = closest_dist;
	
	return true;
}


vec4 compute_atmosphere_color( vec3 ray )
{
	//bool ray_atmosphere_intersection_length( vec3 ray, float sphere_radius, out float len, out bool inside, out float day_evening )
	float through_length;
	bool  is_inside;
	float day_night;
	float closest_distance;
	float total_radius = planet_radius + height;
	bool intersects = ray_atmosphere_intersection_length( ray, total_radius, through_length, closest_distance, is_inside );
	if ( !intersects )
	{
		vec4 c = vec4( color_day.xyz, 0.0 );
		return c;
	}
	//vec4 cc = vec4( 1.0, 0.0, 0.0, 1.0 );
	//return cc;

	// This is for debugging in order to understand what are actual dimensions.
//	vec4 dbg_c = vec4( color_day.xyz, 0.0 );
//	if ( intersects_outer && (through_length_outer > 0.0) )
//		dbg_c.a = 1.0;
	
	// At this point it intersects the very outer surface. 
	// Check if intersects opaque.
	//bool intersects_inner = ray_atmosphere_intersection_length( ray, inner_radius, through_length_inner, closest_distance, inside_inner );
	// This is debug code.
//	if ( intersects_inner && (through_length_inner > 0.0) )
//		dbg_c = vec4( color_night.xyz, 1.0 );
//	return dbg_c;
	
	
	float transparency_dist = (is_inside) ? transparency_distance_inner : transparency_distance_outer;
	float alpha = pow( 0.5, through_length / transparency_dist );
	alpha = 1.0 - alpha;
	//float alpha = through_length_outer / outer_transparency_distance;
	alpha = clamp( alpha, 0.0, 1.0 );
	
	float ray_dot_light = dot( ray, light_dir );
	float night_alpha = clamp( (1.0 + ray_dot_light) * 0.5 + displacement, 0.0, 1.0 );
	alpha *= night_alpha;
	
	float d = clamp( (ray_dot_light + 1.0) * 0.5, 0.0, 1.0 );
	float k_day = d;
	float k_night = 1.0 - d;
	vec4 color = color_day*k_day + color_night*k_night;
	vec4 c = vec4( color.xyz, alpha );
	return c;
}


void vertex()
{
	//sphere_position = (WORLD_MATRIX * vec4(0.0, 0.0, 0.0, 1.0) ).xyz;
	//cam_position = (CAMERA_MATRIX * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
}

void fragment()
{
	//vec3 ray = -(CAMERA_MATRIX * vec4(VIEW, 0.0)).xyz;
	vec3 ray = -VIEW;
	ray = ray / length(ray);
	
	vec4 c = compute_atmosphere_color( ray );
	
	ALBEDO = c.rgb;
	ALPHA  = c.a;
}


