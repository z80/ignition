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
	var sun_se3: Se3Ref = sun.relative_to( rotation )
	var light_dir: Vector3 = sun_se3.r.normalized()
	light_dir = source_se3.q.xform( light_dir )
	
	
	var radius_km: float = surface_source_solid.radius_km
	var scale: float    = 1.0 / config_space.scale_divider
	
	var coeff: float = _scale_dist_ratio.compute_scale( source_se3, scale )
	var t: Transform = _scale_dist_ratio.compute_transform( source_se3, scale )
	self.transform = t
	
	var scale_km: float = coeff * 1000.0;
	
	var planet_radius: float = radius_km * scale_km
	var height: float        = config_atmosphere.height_km * scale_km
	var inner_dist: float    = config_atmosphere.transparency_dist_inner_km * scale_km
	var outer_dist: float    = config_atmosphere.transparency_dist_outer_km * scale_km
	var color_day: Color     = config_atmosphere.atmosphere_color_day
	var color_night: Color   = config_atmosphere.atmosphere_color_night
	var displacement: float  = config_atmosphere.displacement


#	planet_radius += 2.5 * 1000.0
#	inner_height += 1.0 * 1000.0
#	outer_height += 1.0 * 1000.0
#	planet_radius = 1.0
	
	var m: ShaderMaterial = self.get_surface_material( 0 )
	m.set_shader_param( "planet_center",               planet_center )
	m.set_shader_param( "planet_radius",               planet_radius )
	m.set_shader_param( "height",                      height )
	m.set_shader_param( "transparency_distance_inner", inner_dist )
	m.set_shader_param( "transparency_distance_outer", outer_dist )
	m.set_shader_param( "light_dir",                   light_dir )
	m.set_shader_param( "displacement",                displacement )
	m.set_shader_param( "color_day",                   color_day )
	m.set_shader_param( "color_night",                 color_night )

