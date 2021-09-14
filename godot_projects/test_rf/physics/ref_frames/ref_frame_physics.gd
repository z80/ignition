
extends RefFrame
class_name RefFramePhysics

var _physics_env = null

var Clustering = preload( "res://physics/ref_frames/clustering.gd" )

# Surface collision.
var SurfaceProvider = preload( "res://physics/bodies/surface_provider/surface_provider.tscn" )
var _surface_provider = null

# Subdividion source reference in order to determine 
# if it's time to rebuild the sphere visual and collision surface.
# Can collide with just one.
var _subdivide_source_physical: SubdivideSourceRef = null

# This is the thing which computes celestial orbits.
# It is TODO. Not yet really integrated.
var motion: CelestialMotionRef = null


export(bool) var allow_orbiting setget _set_allow_orbiting, _get_allow_orbiting

# For debugging jump only this number of times.
#var _jumps_left: int = 50

var debug_has_split: bool = false


# Override get_class() method in order
# to be able to identify objects of this class.
func get_class():
	return "RefFramePhysics"

func _enter_tree():
	pass


func process_children():
	#print( "******************** process children" )
	.process_children()
	#print( "******************** apply forces" )
	apply_forces()
	#if not debug_has_split:
	#exclude_too_far_bodies()
	#print( "******************** include close enough bodies" )
	include_close_enough_bodies()
	#print( "******************** split if needed" )
	var has_split: bool = split_if_needed()
	debug_has_split = has_split
	if has_split:
		return true
	#print( "******************** merge if needed" )
	if ( merge_if_needed() ):
		return true
	#print( "******************** self delete if unused" )
	if ( self_delete_if_unused() ):
		return true
	
	return false



func launch( gm: float, se3: Se3Ref ):
	if motion != null:
		motion.init( gm, se3 )


func evolve( _dt: float ):
	.evolve( _dt )
	evolve_motion( _dt )
	jump_if_needed()



func evolve_motion( _dt: float ):
	if motion == null:
		return
	# SE3 is assigned internally.
	if _dt > 0.1:
		_dt = 0.1
	motion.process_rf( _dt, self )
	
#	var t: String = motion.movement_type()
#	if t == "idle":
#		return
#	var se3: Se3Ref = motion.se3
#	var r: Vector3 = se3.r
#	var v: Vector3 = se3.v
#	var l: float = motion.specific_angular_momentum()
#	print( "movement type: ", t )
#	print( "spec ang mom:  ", l )
#	print( "r:             ", r )
#	print( "v:             ", v )
#	print( "" )




# Override ready. Added surface provider creation.
func ready():
	.ready()
	_create_physics_environment()
	_create_motion()
	_create_surface_provider()
	_create_subdivide_source()
	
	add_to_group( Constants.REF_FRAME_PHYSICS_GROUP_NAME )



func _create_motion():
	motion = CelestialMotionRef.new()
	motion.allow_orbiting = false


func _create_physics_environment():
	var Env = preload( "res://physics/ref_frames/physics_env/physics_env.tscn" )
	var env = Env.instance()
	var root: Node = RootScene.get_root_for_physics_envs()
	root.add_child( env )
	_physics_env = env


func _destroy_physics_environment():
	if _physics_env != null:
		_physics_env.queue_free()
		_physics_env = null


func _create_surface_provider():
	if _surface_provider != null:
		return
	_surface_provider = SurfaceProvider.instance()
	_surface_provider.init()
	_surface_provider.change_parent( self )


func _create_subdivide_source():
	if _subdivide_source_physical != null:
		return
	
	_subdivide_source_physical = SubdivideSourceRef.new()


# This one is called by Bodies in order to enable physics. 
func add_physics_body( body: RigidBody ):
	if _physics_env != null:
		_physics_env.add_physics_body( body )



func set_surface_vertices( verts: PoolVector3Array, se3: Se3Ref ):
	print( "surface provides is given ", verts.size(), " vertices" )
	if _surface_provider != null:
		_surface_provider.set_vertices( verts, se3 )


func update_surface_vertices( se3: Se3Ref ):
	if _surface_provider != null:
		_surface_provider.update_vertices( se3 )


func init_physics():
	_create_physics_environment()



func finit_physics():
	_destroy_physics_environment()



# If physics is initialized and being processed.
func is_active():
	var en: bool = (_physics_env != null)
	return en












func jump( t: Transform, v: Vector3=Vector3.ZERO ):
	# Debug output.
	var tp: String = motion.movement_type()
	var dbg: bool = (tp != "idle")
	self.debug = dbg
	if dbg:
		print( "" )
		print( "jump with debug output:" )
	
	t.basis = Basis.IDENTITY
	self.set_jump_t( t )
	self.set_jump_v( v )
	self.apply_jump()
	
	# Update SE3 in orbital motion.
	var se3: Se3Ref = self.get_se3()
	motion.se3 = se3

	# Turn debug off and keep with debug output.
	self.debug = false
	
	if tp == "idle":
		return
	se3 = motion.se3
	var r: Vector3 = se3.r
	v = se3.v
	var l: float = motion.specific_angular_momentum()
	print( "movement type: ", t )
	print( "spec ang mom:  ", l )
	print( "r:             ", r )
	print( "v:             ", v )

	var v_accum: Vector3 = Vector3.ZERO
	var bodies: Array = child_bodies( false )
	var qty: int = bodies.size()
	for b in bodies:
		var vv: Vector3 = b.v()
		v_accum += vv
	v_accum /= float( qty )

	print( "" )

	#var after_t: Transform = self.t()

	#var poses_after: Array = []
	#for body in bodies:
	#	var se3: Se3Ref = body.get_se3()
	#	poses_after.push_back( se3.r )
	
	#print( "\n\n\n" )
	#print( "RefFramePhysics jumped from\n", before_t, "\nto\n", after_t )
	#var qty: int = len( bodies )
	#for i in range( qty ):
	#	var name: String = bodies[i].name
	#	print( "body ", name, ": ", poses_before[i], " -> ", poses_after[i] )




# ********************************************
#     Objects management, clustering.
# ********************************************

func jump_if_needed():
	# This is for debugging. Jump only this amount of times.
	#if _jumps_left <= 0:
	#	return
	
	# Compute center of all bodies in the ref frame.
	var bodies: Array = child_bodies( false )
	var qty: int =  bodies.size()
	if qty < 1:
		return
	var r: Vector3 = Vector3.ZERO
	for b in bodies:
		r += b.r()
	r /= float( qty )
	
	var v: Vector3 = Vector3.ZERO
	if self.allow_orbiting:
		for b in bodies:
			if b.get_class() == "PartAssembly":
				qty -= 1
				continue
			v += b.v()
		if qty > 0:
			v /= float( qty )
	
	var dist: float = r.length()
	if dist < Constants.RF_JUMP_DISTANCE:
		return
	
	var t: Transform = Transform.IDENTITY
	t.origin = r
	jump( t, v )
	
	# Enforce collision layer and visual subdivide.
	_subdivide_source_physical.force_subdivide()
	var player_rf: RefFrameNode = PhysicsManager.get_player_ref_frame()
#	if self == player_rf:
#		PhysicsManager.force_rebuild_visual_spheres()
	#_jumps_left -= 1


# Trying to override the default one in order to take into account
# orbital motion.
func set_se3( se3: Se3Ref ):
	if motion != null:
		motion.se3 = se3
	.set_se3( se3 )




# Actually, with modern approach it shouldn't exclude.
# It should split ref frame into two. So this one is probably not used now...
func exclude_too_far_bodies():
	var max_dist: float = Constants.BODY_EXCLUDE_DIST
	var bodies = root_most_child_bodies()
	var player_control = PhysicsManager.player_control
	var pt = self.get_parent()
	
	for body in bodies:
		if body == player_control:
			continue
		
		var r: Vector3 = body.r()
		var d: float = r.length()
		if d > max_dist:
			body.change_parent( pt )



func include_close_enough_bodies():
	var min_dist: float = Constants.BODY_INCLUDE_DIST
	var bodies = parent_bodies()
	
	for body in bodies:
		# This is to make sure that bodies added as a part of a super body 
		# are not added twice.
		var p = body.get_parent()
		if p == self:
			continue
		body.compute_relative_to_root( self )
		var r: Vector3 = body.r_root()
		var d: float = r.length()
		
		if d < min_dist:
			body.change_parent( self )



static func print_all_ref_frames():
	print( "*********************************************************" )
	print( "All ref frames" )
	var player_rf = PhysicsManager.get_player_ref_frame()
	print( "player rf: ", player_rf.name )
	var rfs: Array = PhysicsManager.physics_ref_frames()
	for rf in rfs:
		var se3: Se3Ref = rf.get_se3()
		print( "rf name: ", rf.name, " r: ", se3.r )
		print( "bodies: " )
		var bodies: Array = rf.child_bodies( false )
		for body in bodies:
			var name: String = body.name
			se3 = body.get_se3()
			print( name, ": ", se3.r )
	
	print( "" )


func split_if_needed():
	var bodies = root_most_child_bodies()
	if ( bodies.size() < 2 ):
		return false
	
	var ret: Array = Clustering.cluster( bodies )
	var dist: float    = ret[0]
	var split_ind: int = ret[1]
	var dest: Array    = ret[2]
	var split_dist: float = Constants.RF_SPLIT_DISTANCE
	if dist < split_dist:
		return false
	
	var qty: int = dest.size()
	if ( split_ind < 1 ) or ( split_ind >= qty ):
		return false
	
	print( "\n\n\n" )
	print( "splitting ref frame ", self.name )
	print( "just before split: " )
	print_all_ref_frames()
	#_debug_distances( bodies )
	
	var bodies_a: Array = []
	var bodies_b: Array = []
	for i in range(split_ind):
		var body: RefFrameNode = dest[i]
		bodies_a.push_back( body )
	for i in range( split_ind, qty ):
		var body: RefFrameNode = dest[i]
		bodies_b.push_back( body )
	
	# Check on which side the user controlled body is.
	var player_in_b: bool = false
	for body in bodies_b:
		var has_player: bool = body.has_player_control()
		if has_player:
			# Swap bodies_a  and bodies_b
			var bodies_c: Array = bodies_b
			bodies_b = bodies_a
			bodies_a = bodies_c
			break
	
	# At this point both arrays are not empty and if player ref frame is here, 
	# it is in bodies_a.
	var p = get_parent()
	var rf: RefFrame = PhysicsManager.create_ref_frame_physics()
	rf.change_parent( p )
	var se3: Se3Ref = self.get_se3()
	
#	var accum_r: Vector3 = Vector3.ZERO
#	for b in bodies_b:
#		var se3_b: Se3Ref = b.get_se3()
#		accum_r += se3_b.r
#	accum_r /= float( bodies_b.size() )
#	se3.r = accum_r

	rf.set_se3( se3 )
	
	for body in bodies_b:
		body.change_parent( rf )
	
	print( "new rf created ", rf. name )
	print( "after split: " )
	print_all_ref_frames()
	print( "" )
	
	#For debugging compute distance as if we wanted to merge ref frames.
	#var dist_2: float = distance( rf )
	#_debug_distances( bodies )
	#dist_2 = distance( rf )

	
	return true 


func merge_if_needed():
	var ref_frames: Array = PhysicsManager.physics_ref_frames()
	for rf in ref_frames:
		if rf == self:
			continue
		var queued_for_deletion: bool = rf.is_queued_for_deletion()
		if queued_for_deletion:
			continue
		
		var dist: float = distance( rf )
		if dist < Constants.RF_MERGE_DISTANCE:
			
			print( "\n\n\n" )
			dist = distance( rf )
			print( "merging ", rf.name, " with ", self.name )
			print( "info before" )
			print_all_ref_frames()
			
			var bodies: Array = rf.root_most_child_bodies( false )
			for body in bodies:
				body.change_parent( self )
			
			
			print( "merged ", rf.name, " with ", self.name )
			# Also check if it is player's ref frame.
			# If it is, change it to the one everything is merged to.
			var player_rf: RefFramePhysics = PhysicsManager.get_player_ref_frame()
			if rf == player_rf:
				print( "player ref frame changed to ", rf.name )
			
			# Queue for deletion.
			rf.queue_free()
			
			print( "info after" )
			print_all_ref_frames()
			
			return true
	return false


# Need to be removed if returned "true".
func self_delete_if_unused():
	var bodies: Array = root_most_child_bodies()
	var qty: int = bodies.size()
	if ( qty < 1 ):
		# Also don't delete player ref. frame.
		var player_rf: RefFramePhysics = PhysicsManager.get_player_ref_frame()
		if self == player_rf:
			return false
		self.queue_free()
		return true
	return false



func child_bodies( including_surf_provider: bool = false ):
	var children = get_children()
	var bodies = []
	for ch in children:
		var b = ch as RefFrameNode
		var include: bool = (b != null)
		if not including_surf_provider:
			include = include and (b != _surface_provider)
		if include:
			bodies.push_back( b )
	
	return bodies


func root_most_child_bodies( including_surf_provider: bool = false ):
	var children = get_children()
	var bodies = []
	for ch in children:
		var b = ch as RefFrameNode
		var include: bool = (b != null)
		if not including_surf_provider:
			include = include and (b != _surface_provider)
		var root_most_body: RefFrameNode = b.root_most_body()
		include = include and (not (root_most_body in bodies))
		if include:
			bodies.push_back( b )
	
	return bodies


func parent_bodies():
	var pt = self.get_parent()
	var rt = RootScene.get_root_for_bodies()
	
	var bodies = []
	
	var children = pt.get_children()
	for child in children:
		var body = child as RefFrameNode
		if body != null:
			var cl_name: String = body.get_class()
			if cl_name == "Body":
				body = body.root_most_body()
				if not (body in bodies):
					bodies.push_back( body )
	
	if rt == pt:
		return bodies

	children = rt.get_children()
	for child in children:
		var body = child as RefFrameNode
		if body != null:
			body = body.root_most_body()
			var cl_name: String = body.get_class()
			if cl_name == "Body":
				if not (body in bodies):
					bodies.push_back( body )
	
	return bodies



func apply_forces():
	var rf: RefFrame = closest_force_source()
	if rf == null:
		return
	
	var children = self.get_children()
	for child in children:
		var body = child as RefFrameNode
		if body != null:
			process_body( rf, body )




func process_body( force_source_rf: RefFrameNode, body: RefFrameNode, up_defined: bool = false ):
	force_source_rf.compute_relative_to_root( body )
	var r: Vector3 = force_source_rf.r_root()
	var v: Vector3 = force_source_rf.v_root()
	var q: Quat    = force_source_rf.q_root()
	var w: Vector3 = force_source_rf.w_root()
	
#	# This "if" statement is for debugging.
#	if body.name == "Thruster_01":
#		body.compute_relative_to_root( force_source_rf )
#		var r_rel: Vector3 = body.r_root()
#		var q_rel: Quat    = body.q_root()
#		var qq: Quat = q_rel * q
#		var i = 1
	
	var ret: Array = []
	var fs: ForceSource = force_source_rf.force_source
	var orbiting: bool = self.is_orbiting()
	fs.compute_force( body, orbiting, r, v, q, w, ret )
	if not up_defined:
		var defines_vertical: bool = fs.defines_vertical()
		if defines_vertical:
			up_defined = true
			var p: Node = body.get_parent()
			var rfp: RefFramePhysics = p as RefFramePhysics
			if rfp != null:
				var up: Vector3 = fs.up( force_source_rf, body )
				up = body.q().xform( up )
				if body.has_method( "set_local_up" ):
					body.set_local_up( up )
	
	var F: Vector3 = ret[0]
	var P: Vector3 = ret[1]
	# Convert to physics ref. frame.
	var q_adj: Quat = body.q()

	F = q_adj.xform( F )
	P = q_adj.xform( P )

	# Apply force and torque to the body.
	if body.has_method( "add_force_torque" ):
		body.add_force_torque( F, P )

	# Befor computing own force contribution recursively searches for 
	# other force sources if recursive search is allowed.
	if force_source_rf.force_source.recursive():
		var p_ref_frame: RefFrame = force_source_rf.closest_parent_ref_frame()
		if p_ref_frame != null:
			force_source_rf = p_ref_frame.closest_force_source()
			if force_source_rf != null:
				process_body( force_source_rf, body, up_defined )



func get_subdivide_source():
	return _subdivide_source_physical



func distance( b: RefFramePhysics ):
	var bodies_a: Array = root_most_child_bodies()
	var bodies_b: Array = b.root_most_child_bodies()
	
	#var bodies_all: Array = bodies_a + bodies_b
	#_debug_distances( bodies_all )
	
	var min_d: float = -1.0
	for body_a in bodies_a:
		for body_b in bodies_b:
			var d: float = body_a.distance( body_b )
			if (min_d < 0.0) or (d < min_d):
				min_d = d
	
	return min_d


func _parent_changed():
	var p = get_parent()
	var se3: Se3Ref = self.relative_to( p )
	var allowed: bool = self.allow_orbiting
	if not allowed:
		se3.v = Vector3.ZERO
		se3.w = Vector3.ZERO
	self.jump_to( p, se3 )
	if motion != null:
		motion.se3 = se3




func on_delete():
	finit_physics()
	if _surface_provider != null:
		_surface_provider.queue_free()
		_surface_provider = null
	# It's subclassed from a reference.
	# Should be enough to just remove all references to 
	# make it released.
	_subdivide_source_physical = null
	


func is_orbiting():
	var t: String = motion.movement_type()
	var ret: bool = (t != "idle")
	return ret



func _set_allow_orbiting( en: bool ):
	if motion == null:
		return
	motion.allow_orbiting = en



func _get_allow_orbiting():
	if motion == null:
		return false
	allow_orbiting = motion.allow_orbiting
	return allow_orbiting



func serialize():
	var data: Dictionary = .serialize()
	data.allow_orbiting = self.allow_orbiting
	return data



func deserialize( data: Dictionary ):
	var ret: bool = .deserialize( data )
	if not ret:
		return false
	
	return true



static func _debug_distances( bodies: Array ):
	var bodies_node: Node = RootScene.get_root_for_bodies()
	var tree: SceneTree = bodies_node.get_tree()
	var vp: Viewport = tree.root
	var root_node: Node = vp.get_node( "Root" ).get_node( "Sun" )
	var ass: Body = root_node.find_node( "Construction", true, false )
	print( "" )
	print( "                All relative to assembly:" )
	for b in bodies:
		var body = b as RefFrameNode
		if body == null:
			continue
		var se3: Se3Ref = body.relative_to( ass )
		print( body.name, ": ", se3.r )
	pass




