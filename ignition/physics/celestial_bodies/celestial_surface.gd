
extends CelestialBody
class_name CelestialSurface

# Defining geometry and GM based on surface orbiting velocity.
export(String) var height_source_name = "test"


# Subdivision levels.
export(float) var detail_size_0 = 0.01
export(float) var detail_dist_0 = 0.05
export(float) var detail_size_1 = 0.03
export(float) var detail_dist_1 = 0.2
export(float) var detail_size_2 = 0.05
export(float) var detail_dist_2 = 10.0

export(float) var atmosphere_height_inner_km = 1.0
export(float) var atmosphere_height_outer_km = 0.1
export(float) var transparency_dist_inner_km = 30.0
export(float) var transparency_dist_outer_km = 5.0
export(Color) var atmosphere_color_day   = Color(0.65, 0.8, 1.0, 1.0)
export(Color) var atmosphere_color_night = Color(1.0, 0.43, 0.46, 1.0)
export(float) var displacement = 0.0



var motion: CelestialMotionRef = null
var rotation: CelestialRotationRef = null
var orbit_visualizer: Node = null
export(Color) var orbit_color = Color( 1.0, 0.0, 0.0, 1.0 ) setget _set_orbit_color, _get_orbit_color
export(bool) var show_orbit = false setget _set_show_orbit, _get_show_orbit


# As currently there is just one player
# And even if many one player per PC need just one 
# subdivide source per visual sphere.
var _subdivide_source_visual: SubdivideSourceRef = null


export(bool) var convert_to_global = false setget _set_convert_to_global
export(bool) var apply_scale       = true setget _set_apply_scale


var ref_frame_to_check_rotating_index: int = 0
var ref_frame_to_check_orbiting_index: int = 0


export(float) var rescale_angular_distance = 1.1 / 180.0 * PI
var last_player_rf_r: Vector3 = Vector3.ZERO


var _force_source_rotational: ForceSourceRotational = null


func get_class():
	return "CelestialSurface"


func translation_rf():
	var rf: RefFrameNode = get_node( "." ) as RefFrameNode
	return rf


func rotation_rf():
	var rf: RefFrameNode = get_node( "Rotation" ) as RefFrameNode
	return rf


func surface_node():
	var cube_sphere: CubeSphereNode = get_node( "Rotation/CelestialBody" ) as CubeSphereNode
	return cube_sphere



func _ready():
	._ready()
	# It should be created only when the thing is in the tree.
	_create_orbit_visualizer()



func init():
	if initialized:
		return
	initialized = true
	
	add_to_group( Constants.PLANETS_GROUP_NAME )
	
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
	
	# Initialize rotational force source
	_force_source_rotational = ForceSourceRotational.new()
	_force_source_rotational.ang_velocity = (1.0/(rotation_period_hrs*3600.0))*rotation_axis
	
	# Initialize body geometry
	var celestial_body: CubeSphereNode = get_node( "Rotation/CelestialBody" )
	var radius: float  = radius_km * 1000.0
	var height: float  = height_km * 1000.0
	celestial_body.radius = radius
	celestial_body.height = height
	celestial_body.height_source   = height_source( height_source_name, radius, height )
#	celestial_body.distance_scaler = PhysicsManager.distance_scaler
	
	celestial_body.apply_scale         = true
	celestial_body.convert_to_global   = false
#	celestial_body.scale_mode_distance = 3.0
	
	celestial_body.clear_levels()
	celestial_body.add_level( detail_size_0, detail_dist_0 )
	celestial_body.add_level( detail_size_1, detail_dist_1 )
	celestial_body.add_level( detail_size_2, detail_dist_2 )
	
	.init()



func process( delta: float, force_player_rf: RefFrameNode = null ):
	process_motion( delta )
	process_geometry( force_player_rf )
	#process_rescale( force_player_rf )
	_process_visualize_orbits()



func process_motion( delta ):
	var translation_rf = get_node( "." )
	var rotation_rf    = get_node( "Rotation" )
	motion.process_rf( delta, translation_rf )
	rotation.process_rf( delta, rotation_rf )
	
	#var t: Transform = rotation_rf.transform
	#print( "rotation: ", Quat( t.basis ) )
	#t = translation_rf.transform
	#print( "planet pos: ", t.origin )



func process_geometry( force_player_rf: RefFrameNode = null ):
	var player_rf: RefFrameNode
	var player_ctrl: RefFrameNode
	if force_player_rf != null:
		player_rf = force_player_rf
	else:
		player_rf = PhysicsManager.get_player_ref_frame()
	player_ctrl = PhysicsManager.player_control
	
	var physics_ref_frames: Array  = PhysicsManager.physics_ref_frames()
	
	var translation: RefFrameNode = self
	var rotation: RefFrameNode    = get_node( "Rotation" )
	var planet: CubeSphereNode = get_node( "Rotation/CelestialBody" )
	
	var paths: Array = []
	for rf in physics_ref_frames:
		# Check if either node is direct parent of this rf
		var p = rf.get_parent()
		var is_child: bool = ( p == planet ) or ( p == rotation ) or ( p == translation )
		if not is_child:
			continue
		
		var subdiv: SubdivideSourceRef = rf.get_subdivide_source()
		var need_rebuild: bool = subdiv.need_subdivide( rf, planet )
		if need_rebuild:
			# Build the surface for this particular ref frame physics.
			planet.subdivide_2( rf, subdiv )
			planet.apply_heightmap_2( planet.height_source )
			var collision_dist = Constants.RF_MERGE_DISTANCE
			var surface_relative_to_rf: Se3Ref = planet.relative_to( rf )
			var verts: PoolVector3Array = planet.collision_triangles( rf, subdiv, collision_dist )
			rf.set_surface_vertices( verts, surface_relative_to_rf )
	
	# For player ref frame rebuild mesh if needed
	# Generate visual appearance based on camera.
	var camera: RefFrameNode = PhysicsManager.camera
	if (camera != null) and ( is_instance_valid(camera) ):
		var need_rebuild_visual: bool = _subdivide_source_visual.need_subdivide( camera, planet )
		#print( "need_rebuild_visual: ", need_rebuild_visual )
		if need_rebuild_visual:
			planet.subdivide_2( camera, _subdivide_source_visual )
			planet.apply_heightmap_2( planet.height_source )
			#planet.apply_visual_mesh()

		var distance_scaler: DistanceScalerRef = PhysicsManager.distance_scaler
		planet.apply_scale_2( camera, null, distance_scaler )
		planet.apply_visual_mesh()










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






# Here need to switch between orbiting and being on a rotating surface.
func process_ref_frames( celestial_bodies: Array ):
	.process_ref_frames( celestial_bodies )
	
	# Apply gravity, centrifugal, Coriolis and Euler force to bodies in 
	# rotational ref. frame.
	# (-2*m * w x v); (-m * w x (w x r)); (-m*e x r)
	_apply_rotational_forces()
	
	process_ref_frames_rotating_to_orbiting()
	process_ref_frames_orbiting_to_rotating()
	process_ref_frames_orbiting_change_parent( celestial_bodies )

	ref_frame_to_check_rotating_index += 1
	ref_frame_to_check_orbiting_index += 1




func _apply_rotational_forces():
	var rot: RefFrameNode = rotation_rf()
	var bodies: Array = get_all_physics_bodies( rot )
	
	for b in bodies:
		var se3_rel_to_body: Se3Ref = rot.relative_to( b )
		var se3_local: Se3Ref       = b.get_se3()
		var q: Quat                 = se3_local.q
		
		# Apply gravity.
		var force_torque: Array     = force_source_gravity.compute_force( b, se3_rel_to_body )
		var F: Vector3 = force_torque[0]
		var P: Vector3 = force_torque[1]
		F = q.xform( F )
		P = q.xform( P )
		b.add_force_torque( F, P )
		
		# Apply rotational forces.
		force_torque = _force_source_rotational.compute_force( b, se3_rel_to_body )
		F = force_torque[0]
		P = force_torque[1]
		F = q.xform( F )
		P = q.xform( P )
		b.add_force_torque( F, P )




func process_ref_frames_rotating_to_orbiting():
	var rot: RefFrameNode = rotation_rf()
	var rfs: Array = get_ref_frames( rot )
	var qty: int = rfs.size()
	if qty < 1:
		return
	if ref_frame_to_check_rotating_index >= qty:
		ref_frame_to_check_rotating_index = 0

	var rf: RefFramePhysics = rfs[ref_frame_to_check_rotating_index]
	var tr: RefFrameNode = translation_rf()
	var se3: Se3Ref = rf.relative_to( rot )
	var dist: float = se3.r.length()
	var exclusion_dist: float = (radius_km + height_km)*1000.0 + Constants.BODY_EXCLUDE_DIST
	if dist >= exclusion_dist:
		rf.change_parent( tr )
		#rf.jump_to( tr, se3 )
		rf.allow_orbiting = true
		se3 = rf.get_se3()
		rf.launch( gm, se3 )
		print( "rotating -> orbiting" )
		
		var m: CelestialMotionRef = rf.motion
		var t: String = m.movement_type()
		var l: float = m.specific_angular_momentum()
		se3 = m.se3
		var r: Vector3 = se3.r
		var v: Vector3 = se3.v
		print( "movement type: ", t )
		print( "spec ang mom:  ", l )
		print( "r:             ", r )
		print( "v:             ", v )



func process_ref_frames_orbiting_to_rotating():
	var tr: RefFrameNode = translation_rf()
	var rfs: Array = get_ref_frames( tr )
	var qty: int = rfs.size()
	if qty < 1:
		return
	if ref_frame_to_check_orbiting_index >= qty:
		ref_frame_to_check_orbiting_index = 0
	
	var rf: RefFramePhysics = rfs[ref_frame_to_check_orbiting_index]
	var rot: RefFrameNode = rotation_rf()
	var se3: Se3Ref = rf.relative_to( rot )
	var dist: float = se3.r.length()
	var inclusion_dist: float = (radius_km + height_km)*1000.0 + Constants.BODY_INCLUDE_DIST
	if dist <= inclusion_dist:
		rf.change_parent( rot )
		rf.allow_orbiting = false
		print( "orbiting -> rotating" )


func process_ref_frames_orbiting_change_parent( celestial_bodies: Array ):
	var tr: RefFrameNode = translation_rf()
	var rfs: Array = get_ref_frames( tr )
	var qty: int = rfs.size()
	if qty < 1:
		return
	if ref_frame_to_check_orbiting_index >= qty:
		ref_frame_to_check_orbiting_index = 0
	
	var rf: RefFramePhysics = rfs[ref_frame_to_check_orbiting_index]
	# Determine distance to all bodies.
	var celestial_bodies_qty: int = celestial_bodies.size()
	var biggest_influence: float = -1.0
	var biggest_influence_body: CelestialBody = null
	for i in range( celestial_bodies_qty ):
		var cb: CelestialBody = celestial_bodies[i]
		var se3: Se3Ref = rf.relative_to( cb )
		var infl: float = cb.gravitational_influence( se3 )
		if is_nan(infl) or is_inf(infl):
			var iii: int = 0
			rf.debug = true
			var motion_se3: Se3Ref = rf.motion.se3
			var rf_se3: Se3Ref     = rf.get_se3()
			var stri: String = rf.motion.movement_type()
			se3 = rf.relative_to( cb )
			rf.debug = false
		
		if (biggest_influence_body == null) or (biggest_influence < infl):
			biggest_influence = infl
			biggest_influence_body = cb
	
	# Check if the strongest influence is caused by other celestial body.
	if (biggest_influence_body != null) and (biggest_influence_body != tr):
		# Need to teleport celestial body to that other celestial body
		rf.change_parent( biggest_influence_body )
		rf.allow_orbiting = true
		var se3: Se3Ref = rf.get_se3()
		rf.launch( biggest_influence_body.gm, se3 )
		print( "orbiting -> another orbiting" )
		print( "biggest influence obj: ", biggest_influence_body.name )
		print( "biggest influence gm: ", biggest_influence_body.gm )


func _create_orbit_visualizer():
	if is_instance_valid( orbit_visualizer ):
		orbit_visualizer.queue_free()
	var Vis = load( "res://physics/celestial_bodies/orbit_visualizer.tscn" )
	orbit_visualizer = Vis.instance()
	self.add_child( orbit_visualizer )
	orbit_visualizer.ref_frame = self.get_parent()
	orbit_visualizer.motion    = motion
	orbit_visualizer.color     = orbit_color



func _set_orbit_color( c: Color ):
	orbit_color = c
	if orbit_visualizer != null:
		orbit_visualizer.color = orbit_color

func _get_orbit_color():
	return orbit_color

func _set_show_orbit( en: bool ):
	show_orbit = en
	orbit_visualizer.visible = show_orbit
	orbit_visualizer.color   = orbit_color


func _get_show_orbit():
	return show_orbit


func _process_visualize_orbits():
	var new_state: bool = PhysicsManager.visualize_orbits
	var current_state: bool = self.show_orbit
	if current_state != new_state:
		self.show_orbit = new_state
	if show_orbit:
		orbit_visualizer.draw()


