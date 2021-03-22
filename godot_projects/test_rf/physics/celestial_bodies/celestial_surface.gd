
extends CelestialBody
class_name CelestialSurface

# Defining geometry and GM based on surface orbiting velocity.
export(String) var height_source_name = "test"


# Subdivision levels.
export(float) var detail_size_0 = 10.0
export(float) var detail_dist_0 = 100.0
export(float) var detail_size_1 = 50.0
export(float) var detail_dist_1 = 1000.0
export(float) var detail_size_2 = 300.0
export(float) var detail_dist_2 = 1e10

var motion: CelestialMotionRef = null
var rotation: CelestialRotationRef = null

# As currently there is just one player
# And even if many one player per PC need just one 
# subdivide source per visual sphere.
var _subdivide_source_visual: SubdivideSourceRef = null


export(bool) var convert_to_global = false setget _set_convert_to_global
export(bool) var apply_scale       = true setget _set_apply_scale





func translation_rf():
	var rf: RefFrameNode = get_node( "." ) as RefFrameNode
	return rf


func rotation_rf():
	var rf: RefFrameNode = get_node( "Rotation" ) as RefFrameNode
	return rf


func surface_node():
	var cube_sphere: CubeSphereNode = get_node( "Rotation/CelestialBody" ) as CubeSphereNode
	return cube_sphere




func init():
	if initialized:
		return
	initialized = true
	
	add_to_group( Constants.SPHERES_GROUP_NAME )
	
	_subdivide_source_visual = SubdivideSourceRef.new()
	
	motion = CelestialMotionRef.new()
	
	# Initialize GM.
	gm = motion.init_gm( radius_km, surface_orbital_vel_kms )
	
	# Initialize orbital movement.
	perigee_dir = perigee_dir.normalized()
	perigee_vel = perigee_vel - perigee_vel.project( perigee_dir )
	perigee_vel = perigee_vel.normalized()
	
	var p = get_parent()
	var parent_body: CelestialBody = p as CelestialBody
	if parent_body != null:
		parent_body.init()
		var parent_gm: float = parent_body.gm
		motion.launch_elliptic( parent_gm, perigee_dir, perigee_vel, orbital_period_hrs, orbital_eccentricity )
	
	# Initialize rotation.
	rotation = CelestialRotationRef.new()
	rotation.init( rotation_axis, rotation_period_hrs )
	
	# Initialize body geometry
	var celestial_body: CubeSphereNode = get_node( "Rotation/CelestialBody" )
	var radius: float  = radius_km * 1000.0
	var height: float  = height_km * 1000.0
	celestial_body.radius = radius
	celestial_body.height = height
	celestial_body.height_source   = height_source( height_source_name, radius, height )
	celestial_body.distance_scaler = PhysicsManager.distance_scaler
	
	celestial_body.apply_scale         = true
	celestial_body.convert_to_global   = false
	celestial_body.scale_mode_distance = 3.0
	
	celestial_body.clear_levels()
	celestial_body.add_level( detail_size_0, detail_dist_0 )
	celestial_body.add_level( detail_size_1, detail_dist_1 )
	celestial_body.add_level( detail_size_2, detail_dist_2 )
	
	.init()



func process( delta: float, force_player_rf: RefFrame = null ):
	process_motion( delta )
	process_geometry( force_player_rf )



func process_motion( delta ):
	var translation_rf = get_node( "." )
	var rotation_rf    = get_node( "Rotation" )
	motion.process_rf( delta, translation_rf )
	rotation.process_rf( delta, rotation_rf )
	
	#var t: Transform = rotation_rf.transform
	#print( "rotation: ", Quat( t.basis ) )
	#t = translation_rf.transform
	#print( "planet pos: ", t.origin )



func process_geometry( force_player_rf: RefFrame = null ):
	var player_rf: RefFrame
	if force_player_rf != null:
		player_rf = force_player_rf
	else:
		player_rf = PhysicsManager.get_player_ref_frame()
	
	var physics_ref_frames: Dictionary  = PhysicsManager.physics_ref_frames()
	
	var translation: RefFrameNode = self
	var rotation: RefFrameNode    = get_node( "Rotation" )
	var planet: CubeSphereNode = get_node( "Rotation/CelestialBody" )
	
	var paths: Array = []
	for k in physics_ref_frames:
		var rf: RefFramePhysics = physics_ref_frames[k]
		
		# Check if either node is direct parent of this rf
		var p = rf.get_parent()
		var is_child: bool = ( p == planet ) or ( p == rotation ) or ( p == translation )
		if not is_child:
			continue
		
		var subdiv: SubdivideSourceRef = rf.get_subdivide_source()
		var need_rebuild: bool = subdiv.need_subdivide( rf, planet )
		if need_rebuild:
			# Build the surface for this particular ref frame physics.
			planet.rebuild_shape( rf, subdiv )
			var collision_dist = Constants.RF_MERGE_DISTANCE
			var surface_relative_to_rf: Se3Ref = planet.relative_to( rf )
			var verts: PoolVector3Array = planet.collision_triangles( rf, subdiv, collision_dist )
			rf.set_surface_vertices( verts, surface_relative_to_rf )
		else:
			# Need to update current pose for surface vertices as 
			# planet position might have changed if ref. frame is not 
			# a child of the rotation part.
			#var surface_relative_to_rf: Se3Ref = planet.relative_to( rf )
			#rf.update_surface_vertices( surface_relative_to_rf )
			pass
	
	# For player ref frame rebuild mesh if needed
	if player_rf != null:
		var need_rebuild_visual: bool = _subdivide_source_visual.need_subdivide( player_rf, planet )
		#print( "need_rebuild_visual: ", need_rebuild_visual )
		if need_rebuild_visual:
			planet.rebuild_shape( player_rf, _subdivide_source_visual )
			planet.apply_visual_mesh()
		planet.relocate_mesh( player_rf, _subdivide_source_visual )










func height_source( name: String, radius: float, height: float ):
	var HS = preload( "res://physics/celestial_bodies/height_sources.gd" )
	var hs: HeightSourceRef = HS.height_source( name, radius, height )
	return hs




func _set_convert_to_global( en ):
	var celestial_body: CubeSphereNode = get_node( "Rotation/CelestialBody" )
	celestial_body.convert_to_global = en


func _set_apply_scale( en ):
	var celestial_body: CubeSphereNode = get_node( "Rotation/CelestialBody" )
	celestial_body.apply_scale = en



