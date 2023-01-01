
extends CelestialBody
class_name CelestialSurfaceVoxel

# Radius is taken from
var radius_km = 1.0
export(float) var height_km = 1.0

export(float) var atmosphere_height_inner_km = 1.0
export(float) var atmosphere_height_outer_km = 0.1
export(float) var transparency_dist_inner_km = 30.0
export(float) var transparency_dist_outer_km = 5.0
export(Color) var atmosphere_color_day   = Color(0.65, 0.8, 1.0, 1.0)
export(Color) var atmosphere_color_night = Color(1.0, 0.43, 0.46, 1.0)
export(float) var displacement = 0.0

export(float) var air_density          = 1.0
export(float) var air_viscosity        = 0.1
export(float) var air_pressure_surface = 101000.0

var orbit_visualizer: Node = null
export(Color) var orbit_color = Color( 1.0, 0.0, 0.0, 1.0 ) setget _set_orbit_color, _get_orbit_color
export(bool) var show_orbit = false setget _set_show_orbit, _get_show_orbit



export(bool) var convert_to_global = false setget _set_convert_to_global
export(bool) var apply_scale       = true setget _set_apply_scale


var ref_frame_to_check_rotating_index: int = 0
var ref_frame_to_check_orbiting_index: int = 0


export(float) var rescale_angular_distance = 1.1 / 180.0 * PI
var last_player_rf_r: Vector3 = Vector3.ZERO


var _force_source_air_drag: ForceSourceAirDrag      = null

var _translation: RefFrameNode = null
var _rotation: RefFrameNode    = null
var _all_surfaces: Node        = null

func get_class():
	return "CelestialSurfaceVoxel"


func translation_rf():
	if _translation == null:
		_translation = get_node( "." ) as RefFrameNode
	return _translation


func rotation_rf():
	if _rotation == null:
		_rotation = get_node( "Rotation" ) as RefFrameNode
	return _rotation


func surfaces_node():
	if _all_surfaces == null:
		var rot: Node = rotation_rf()
		_all_surfaces = rot.get_child( 0 )
	return _all_surfaces



func _ready():
	var sn: Node = surfaces_node()
	var src: Resource = sn.get_surface_source()
	radius_km = src.source_radius * 0.001

	init_forces()
	
	# It should be created only when the thing is in the tree.
	_create_orbit_visualizer()
	



func init_forces():
	if initialized:
		return
	initialized = true
	
	add_to_group( Constants.PLANETS_GROUP_NAME )
	
	# Initialize GM.
	var gm: float = self.compute_gm_by_speed( radius_km, surface_orbital_vel_kms )
	self.set_own_gm( gm )
	
	# Initialize orbital movement.
	perigee_dir = perigee_dir.normalized()
	perigee_vel = perigee_vel - perigee_vel.project( perigee_dir )
	perigee_vel = perigee_vel.normalized()
	
	var p = get_parent()
	var parent_body: CelestialBody = p as CelestialBody
	if parent_body != null:
		parent_body.init_forces()
		self.launch_elliptic( perigee_dir, perigee_vel, orbital_period_hrs, orbital_eccentricity )
	
	var rot: RefFrameRotationNode = rotation_rf()
	#rot.init( rotation_axis, rotation_period_hrs )
	
	# Initialize force source for air drag forces.
	_init_force_source_air_drag()
	
	.init_forces()



func process( delta: float, force_player_rf: RefFrameNode = null ):
	process_geometry( force_player_rf )
	#process_rescale( force_player_rf )
	_process_visualize_orbits()






func process_geometry( force_player_rf: RefFrameNode = null ):
	var root: RefFrameRoot = RootScene.ref_frame_root
	var player_rf: RefFrameNode
	var player_ctrl: RefFrameNode
	if force_player_rf != null:
		player_rf = force_player_rf
	else:
		player_rf = root.get_player_ref_frame()
	player_ctrl = root.player_control
	
	var physics_ref_frames: Array  = root.physics_ref_frames()
	
	var translation: RefFrameNode = self
	var rotation: RefFrameNode    = rotation_rf()
	var planet: Spatial           = surfaces_node()
	
	var paths: Array = []
	for rf in physics_ref_frames:
		# Check if either node is direct parent of this rf
		var p = rf.get_parent()
		var is_child: bool = ( p == planet ) or ( p == rotation ) or ( p == translation )
		if not is_child:
			continue
		
#		var subdiv: SubdivideSourceRef = rf.get_subdivide_source()
#		var need_rebuild: bool = subdiv.need_subdivide( rf, planet )
#		if need_rebuild:
#			# Build the surface for this particular ref frame physics.
#			planet.subdivide_2( rf, subdiv )
#			planet.apply_heightmap_2( planet.height_source )
#			var collision_dist = Constants.RF_MERGE_DISTANCE
#			var surface_relative_to_rf: Se3Ref = planet.relative_to( rf )
#			var verts: PoolVector3Array = planet.collision_triangles( rf, subdiv, collision_dist )
#			rf.set_surface_vertices( verts, surface_relative_to_rf )
	
	# For player ref frame rebuild mesh if needed
	# Generate visual appearance based on camera.
	var camera: RefFrameNode = root.player_camera
	if (camera != null) and ( is_instance_valid(camera) ):
		var rot: RefFrameNode = rotation_rf()
		var source_se3: Se3Ref = rot.relative_to( camera )
		var surfaces: Node = surfaces_node()
		surfaces.update_source_se3( source_se3 )










func height_source( name: String, radius: float, height: float ):
	var HS = preload( "res://physics/celestial_bodies/height_sources.gd" )
	var hs: HeightSourceRef = HS.height_source( name, radius, height )
	return hs




func _set_convert_to_global( en ):
	var n: Node = get_node( "Rotation/CelestialBody" )
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
	_apply_forces()
	_draw_all_shock_waves()
	
	process_ref_frames_rotating_to_orbiting()
	process_ref_frames_orbiting_to_rotating()
	process_ref_frames_orbiting_change_parent( celestial_bodies )

	ref_frame_to_check_rotating_index += 1
	ref_frame_to_check_orbiting_index += 1




func _apply_forces():
	var rot: RefFrameNode = rotation_rf()
	var bodies: Array = get_all_physics_bodies( rot )
	
	set_local_up( bodies, rot )
	
	for b in bodies:
#		var debug: bool = (b.name == "fuel_tank_2m_1m")
#		if debug:
#			rot.debug = true
		var tr: RefFrameNode = translation_rf()
		var se3_tr: Se3Ref = tr.relative_to( b )
		var se3_rel_to_body: Se3Ref = rot.relative_to( b )
		if debug:
			rot.debug = false
		var se3_local: Se3Ref       = b.get_se3()
		var q: Quat                 = se3_local.q
		
		# Apply gravity and rotational forces.
		var acc: Vector3 = b.acceleration
		var mass: float  = b.get_mass()
		var F: Vector3 = acc * mass
		var P: Vector3 = Vector3.ZERO
		b.add_force_torque( F, P )
		
		# Apply air drag forces.
		var force_torque: Array = _force_source_air_drag.compute_force( b, se3_rel_to_body )
		F = force_torque[0]
		P = force_torque[1]
		F = q.xform( F )
		P = q.xform( P )
		b.add_force_torque( F, P )



func _draw_shock_waves( bodies_ref_frame: RefFrameNode, rot: RefFrameNode ):
	var all_bodies: Array = get_all_physics_bodies( bodies_ref_frame )
	# All assemblies.
	var assemblies: Array = []
	for body in all_bodies:
		var p: PhysicsBodyBase = body as PhysicsBodyBase
		if p == null:
			continue
		var a: PartAssembly = p.get_super_body()
		if a == null:
			continue
		var has: bool = assemblies.has( a )
		if has:
			continue
		
		assemblies.push_back( a )
	
	# For each assembly
	for ass in assemblies:
		var a: PartAssembly = ass as PartAssembly
		var p: Node = a.get_parent()
		var ref_frame: RefFramePhysics = p as RefFramePhysics
		if ref_frame == null:
			continue
		
		var bodies: Array = a.sub_bodies
		var octree_meshes: Array = []
		for b in bodies:
			var body: PhysicsBodyBase = b as PhysicsBodyBase
			var om: OctreeMeshGd = body.get_octree_mesh()
			if om == null:
				continue
			octree_meshes.push_back( om )
		
		var broad_tree: BroadTreeGd = ref_frame.get_broad_tree()
		
		# For each body compute velocity with respect to self.
		if not octree_meshes.empty():
			for b in bodies:
				var body: PhysicsBodyBase = b as PhysicsBodyBase
				
				var se3: Se3Ref = body.relative_to( rot )
				
				var distance: float = se3.r.length()
				# Compute air density in the point.
				var ret: Array = _force_source_air_drag.density_viscosity( se3 )
				var density: float = ret[1]
				
				var v_in_rotation: Vector3 = se3.v
				#var v_in_rotation: Vector3 = Vector3(60.0, 0.0, 0.0)
				var q: Quat     = se3.q
				var inv_q: Quat = q.inverse()
				# Convert this velocity to body's ref. frame.
				var v_in_mesh: Vector3 = inv_q.xform( v_in_rotation )
				#var v_in_mesh: Vector3 = q.xform( v_in_rotation )
				
				# Body in physics ref. frame.
				var se3_mesh_to_rf: Se3Ref = body.relative_to( ref_frame )
				var se3_rf_to_mesh: Se3Ref = se3_mesh_to_rf.inverse()
				
				# broad_tree, otree_meshes, density, vel_in_mesh, se3_mesh_to_rf, se3_rf_to_mesh
				body.update_shock_wave_visual( broad_tree, octree_meshes, density, v_in_mesh, se3_mesh_to_rf, se3_rf_to_mesh )




func _draw_all_shock_waves():
	var rot: RefFrameNode = rotation_rf()
	var tran: RefFrameNode = translation_rf()
	var all_bodies: Array = get_all_physics_bodies( rot )
	_draw_shock_waves( rot,  rot )
	_draw_shock_waves( tran, rot )



func _hide_shock_waves():
	var rot: RefFrameNode = rotation_rf()
	var all_bodies: Array = get_all_physics_bodies( rot )
	# All assemblies.
	var assemblies: Array = []
	for body in all_bodies:
		var p: PhysicsBodyBase = body as PhysicsBodyBase
		p.hide_shock_wave_visual()




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
		rf.set_se3( se3 )
		rf.launch()
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
		rf.set_se3( se3 )
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
	# First, compute distance to own center.
	var se3_self: Se3Ref = rf.relative_to( self )
	var own_distance: float = se3_self.r.length()
	
	# If not already computed it will be computed here.
	compute_min_shpere_of_influence_to_parent()
	var not_too_far: bool = (own_distance < _min_sphere_of_influence_to_parent)
	
	# Determine distance to all bodies.
	var celestial_bodies_qty: int = celestial_bodies.size()
	var biggest_influence: float = -1.0
	var biggest_influence_body: CelestialBody = null
	for i in range( celestial_bodies_qty ):
		var cb: CelestialBody = celestial_bodies[i]
		var se3: Se3Ref = rf.relative_to( cb )
		# Check if the body is in parents list.
		# And don't try to switch to it if it's one of parents 
		# and we are not far enough yet.
		if _all_parents.has( cb ) and not_too_far:
			# Don't even compute influence of this body.
			continue
		
		var infl: float = cb.gravitational_influence( se3 )
#		if is_nan(infl) or is_inf(infl):
#			var iii: int = 0
#			rf.debug = true
#			var motion_se3: Se3Ref = rf.motion.se3
#			var rf_se3: Se3Ref     = rf.get_se3()
#			var stri: String = rf.motion.movement_type()
#			se3 = rf.relative_to( cb )
#			rf.debug = false
		
		if (biggest_influence_body == null) or (biggest_influence < infl):
				
			biggest_influence = infl
			biggest_influence_body = cb
	
	# Check if the strongest influence is caused by other celestial body.
	if (biggest_influence_body != null) and (biggest_influence_body != tr):
		# Need to teleport celestial body to that other celestial body
		rf.change_parent( biggest_influence_body )
		rf.allow_orbiting = true
		rf.launch()
		print( "orbiting -> another orbiting" )
		print( "biggest influence obj: ", biggest_influence_body.name )
		print( "biggest influence gm: ", biggest_influence_body.own_gm )


func _create_orbit_visualizer():
	if is_instance_valid( orbit_visualizer ):
		orbit_visualizer.queue_free()
	var Vis = load( "res://physics/celestial_bodies/orbit_visualizer.tscn" )
	orbit_visualizer = Vis.instance()
	self.add_child( orbit_visualizer )
	orbit_visualizer.ref_frame = self.get_parent()
	orbit_visualizer.motion    = self
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
	var new_state: bool = RootScene.ref_frame_root.visualize_orbits
	var current_state: bool = self.show_orbit
	if current_state != new_state:
		self.show_orbit = new_state
	if show_orbit:
		orbit_visualizer.draw()



func _init_force_source_air_drag():
	_force_source_air_drag = ForceSourceAirDrag.new()
	_force_source_air_drag.ground_level  = radius_km * 1000.0
	_force_source_air_drag.atm_height    = (atmosphere_height_inner_km + atmosphere_height_outer_km) * 1000.0
	_force_source_air_drag.density_gnd   = air_density * 0.001
	_force_source_air_drag.viscosity_gnd = air_viscosity * 0.001




func air_pressure( se3_rel: Se3Ref ):
	var d: float = se3_rel.r.length() * 0.001 - radius_km
	if d <= 0.0:
		return air_pressure_surface
	
	var atm_height_km: float = atmosphere_height_inner_km + atmosphere_height_outer_km
	if d > atm_height_km:
		return 0.0
	
	var p: float = air_pressure_surface * ( 1.0 - (d / atm_height_km) )
	return p



