extends MeshInstance

export(Resource) var config_atmosphere = null
# This one is for space scale.
export(Resource) var config_space = null
# This one is for planet radius.
export(Resource) var surface_source_solid = null

var _scale_dist_ratio: ScaleDistanceRatioGd = null


# Called when the node enters the scene tree for the first time.
func _ready():
	_scale_dist_ratio = ScaleDistanceRatioGd.new()
	_scale_dist_ratio.max_distance = config_space.event_horizon


func update( rotation: RefFrameRotationNode ):
	var camera: RefFrameNode = RootScene.ref_frame_root.player_camera
	var sun: RefFrameNode    = RootScene.ref_frame_root
	
	var source_se3: Se3Ref = rotation.relative_to( camera )
	var planet_center: Vector3 = source_se3.r
	
	# Relative to the Sun.
	var se3: Se3Ref = rotation.relative_to( sun )
	se3 = se3.inverse()
	var light_dir: Vector3 = se3.r.normalized()
	
	
	var radius_m: float = surface_source_solid.source_radius
	var scale: float    = 1.0 / config_space.scale_divider
	
	var t: Transform = _scale_dist_ratio.compute_transform( source_se3, scale )
	self.transform = t
	
	
	var planet_radius: float = radius_m * 0.001
	var inner_height: float = config_atmosphere.height_inner_km
	var outer_height: float = config_atmosphere.height_total_km - config_atmosphere.height_inner_km 
	var inner_dist: float   = config_atmosphere.transparency_dist_inner_km * 1000.0
	var outer_dist: float   = config_atmosphere.transparency_dist_outer_km * 1000.0
	var color_day: Color    = config_atmosphere.atmosphere_color_day
	var color_night: Color  = config_atmosphere.atmosphere_color_night
	var displacement: float = config_atmosphere.displacement


#	planet_radius += 2.5 * 1000.0
#	inner_height += 1.0 * 1000.0
#	outer_height += 1.0 * 1000.0
#	planet_radius = 1.0
	
	var m: ShaderMaterial = self.get_surface_material( 0 )
	m.set_shader_param( "planet_center",               planet_center )
	m.set_shader_param( "planet_radius",               planet_radius )
	m.set_shader_param( "light_dir",                   light_dir )
	m.set_shader_param( "inner_height",                inner_height )
	m.set_shader_param( "outer_height",                outer_height )
	m.set_shader_param( "inner_transparency_distance", inner_dist )
	m.set_shader_param( "outer_transparency_distance", outer_dist )
	m.set_shader_param( "displacement",                displacement )
	m.set_shader_param( "color_day",                   color_day )
	m.set_shader_param( "color_night",                 color_night )

