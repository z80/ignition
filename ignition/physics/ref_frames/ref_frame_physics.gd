
extends RefFrame
class_name RefFramePhysics

var _physics_env = null

var Clustering = preload( "res://physics/ref_frames/clustering.gd" )

# This is the thing which computes celestial orbits.
# It is TODO. Not yet really integrated.
var motion: CelestialMotionRef = null

# Octree borad phase for mesh queries.
var _broad_tree: BroadTreeGd = null


export(bool) var allow_orbiting setget _set_allow_orbiting, _get_allow_orbiting

# For debugging jump only this number of times.
#var _jumps_left: int = 50

var debug_has_split: bool = false


# Override get_class() method in order
# to be able to identify objects of this class.
func get_class():
	return "RefFramePhysics"


func _init():
	_broad_tree = BroadTreeGd.new()


func _enter_tree():
	pass


func get_broad_tree():
	return _broad_tree


func process_children():
	#print( "******************** process children" )
	.process_children()
	#print( "******************** apply forces" )
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
	
	var ok: bool = _broad_tree.subdivide( self )
	if not ok:
		print( "ERROR: failed to subdivide BroadTreeGd" )



func evolve_motion( _dt: float ):
	if motion == null:
		return
	# SE3 is assigned internally.
#	if _dt > 0.1:
#		_dt = 0.1

	# This is for debugging.
#	var root = get_node( "/root/RefFrameRoot" )
#	PhysicsManager.camera.debug = true
#	var se3 = PhysicsManager.camera.relative_to( root )
#	PhysicsManager.camera.debug = false
	
	motion.process_rf( _dt, self )

	# This is for debugging.
#	root = get_node( "/root/RefFrameRoot" )
#	PhysicsManager.camera.debug = true
#	se3 = PhysicsManager.camera.relative_to( root )
#	PhysicsManager.camera.debug = false
	
#	var n: String = self.name
#	if n != "rf_p for my_character":
#		return
#
#	var planet: RefFrameNode = get_node( "/root/Root/Sun/Planet" )
#	var path: String = planet.get_path()
#	var se3_p: Se3Ref = self.relative_to( planet )
#
#	var t: String = motion.movement_type()
#	print( "" )
#	print( "motion analysis" )
#	print( "movement type: ", t )
#	print( "distance to planet: ", se3_p.r.length() )
#	if t == "idle":
#		return
#	var se3: Se3Ref = motion.se3
#	var r: Vector3 = se3.r
#	var v: Vector3 = se3.v
#	var l: float = motion.specific_angular_momentum()
#	print( "spec ang mom:  ", l )
#	print( "r:             ", r )
#	print( "v:             ", v )




# Override ready. Added surface provider creation.
func ready():
	.ready()
	_create_physics_environment()
	_create_motion()
	
	add_to_group( Constants.REF_FRAME_PHYSICS_GROUP_NAME )



func _create_motion():
	motion = CelestialMotionRef.new()
	motion.allow_orbiting = false
	motion.se3 = self.get_se3()


func _create_physics_environment():
	var Env = preload( "res://physics/ref_frames/physics_env/physics_env.tscn" )
	var env = Env.instance()
	var root: Node = RootScene.get_root_for_physics_envs()
	root.add_child( env )
	_physics_env = env



func get_physics_environment():
	return _physics_env



func _destroy_physics_environment():
	if _physics_env != null:
		_physics_env.queue_free()
		_physics_env = null


# This one is called by Bodies in order to enable physics. 
func add_physics_body( body: RigidBody ):
	if _physics_env != null:
		_physics_env.add_physics_body( body )




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
	var dbg: bool = false
	self.debug = dbg
	if dbg:
		DDD.important()
		DDD.print( "jump with debug output:" )
	
	var elliptic_motion: bool = (motion != null) and (motion.movement_type() == "elliptic")
	if elliptic_motion:
		var n: RefFrameNode = get_node( "/root/Root/Sun/Planet" )
		var se3: Se3Ref = self.relative_to( n )
		DDD.important()
		DDD.print( "relative_to_planet before jump: " + str(se3.r) + ", v: " + str(se3.v) )
	
	t.basis = Basis.IDENTITY
	self.set_jump_t( t )
	self.set_jump_v( v )
	self.apply_jump()
	
	if elliptic_motion:
		var n: RefFrameNode = get_node( "/root/Root/Sun/Planet" )
		var se3: Se3Ref = self.relative_to( n )
		DDD.important()
		DDD.print( "relative_to_planet after jump: " + str(se3.r) + ", v: " + str(se3.v) )

	# Update SE3 in orbital motion.
	var se3: Se3Ref = self.get_se3()
	if elliptic_motion:
		DDD.print( "motion type just before assigning se3 to motion: " + motion.movement_type() )
	motion.se3 = se3
	
	if elliptic_motion:
		DDD.print( "motion type after assigning se3 to motion: " + motion.movement_type() )
		se3 = motion.se3
		DDD.print( "se3 right after assigning se3 to motion: " + str(se3.r) + " v: " + str(se3.v) )


	# Turn debug off and keep with debug output.
	self.debug = false
	
#	if tp == "idle":
#		return
	se3 = motion.se3
	var r: Vector3 = se3.r
	v = se3.v
	var l: float = motion.specific_angular_momentum()
	DDD.print( "movement type: " + str(tp) )
	DDD.print( "spec ang mom:  " + str(l) )
	DDD.print( "r:             " + str(r) )
	DDD.print( "v:             " + str(v) )

#	var v_accum: Vector3 = Vector3.ZERO
#	var bodies: Array = child_bodies( false )
#	var qty: int = bodies.size()
#	for b in bodies:
#		var vv: Vector3 = b.v()
#		v_accum += vv
#	v_accum /= float( qty )

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
	var bodies: Array = child_bodies()
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
	DDD.important()
	DDD.print( "All ref frames" )
	var player_rf = PhysicsManager.get_player_ref_frame()
	if (player_rf != null) and ( is_instance_valid(player_rf) ):
		DDD.print( "player rf: " + player_rf.name )
	var rfs: Array = PhysicsManager.physics_ref_frames()
	for rf in rfs:
		var se3: Se3Ref = rf.get_se3()
		DDD.important()
		DDD.print( "rf name: " + rf.name + " r: " + str(se3.r) )
		DDD.print( "bodies: " )
		var bodies: Array = rf.child_bodies( false )
		for body in bodies:
			var name: String = body.name
			se3 = body.get_se3()
			DDD.print( name + ": " + str(se3.r) )


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
	
	DDD.important()
	DDD.print( "splitting ref frame " + self.name )
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
	
	DDD.important()
	DDD.print( "new rf created " + rf. name )
	DDD.print( "after split: " )
	print_all_ref_frames()
	
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
			
			dist = distance( rf )
			DDD.important()
			DDD.print( "merging " + rf.name + " with " + self.name )
			DDD.print( "info before" )
			print_all_ref_frames()
			
			var bodies: Array = rf.root_most_child_bodies( false )
			for body in bodies:
				body.change_parent( self )
			
			
			DDD.print( "merged " + rf.name + " with " + self.name )
			# Also check if it is player's ref frame.
			# If it is, change it to the one everything is merged to.
			var player_rf: RefFramePhysics = PhysicsManager.get_player_ref_frame()
			if rf == player_rf:
				DDD.print( "player ref frame changed to " + rf.name )
			
			# Queue for deletion.
			rf.queue_free()
			
			DDD.print( "info after" )
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
		
		# And don't delete if parenting the camera.
		var cam: RefFrameNode = PhysicsManager.camera
		if is_instance_valid( cam ):
			var p: Node = cam.get_parent()
			if p == self:
				# In this case may be at most stop orbiting to not 
				# fall inside if a planet
				return false
		
		self.queue_free()
		return true
	return false


# Only derivatives of "PhysicsBodyBase" for applying forces.
func child_physics_bodies():
	var children = get_children()
	var bodies: Array = []
	for ch in children:
		var b: PhysicsBodyBase = ch as PhysicsBodyBase
		var include: bool = (b != null)
		if not include:
			continue
		
		bodies.push_back( b )
	
	return bodies


func child_bodies():
	var children = get_children()
	var bodies = []
	for ch in children:
		var b = ch as RefFrameNode
		var include: bool = (b != null)
		if ch == PhysicsManager.camera:
			continue
		if include:
			bodies.push_back( b )
	
	return bodies


func root_most_child_bodies():
	var children = get_children()
	var bodies = []
	for ch in children:
		var b = ch as RefFrameNode
		var include: bool = (b != null)
		# Don't use camera.
		if ch == PhysicsManager.camera:
			continue
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
		if body == null:
			continue
		if body == PhysicsManager.camera:
			continue
		var cl_name: String = body.get_class()
		if (cl_name == "PhysicsBodyBase") or (cl_name == "Part"):
			body = body.root_most_body()
			if not (body in bodies):
				bodies.push_back( body )
	
	if rt == pt:
		return bodies

	children = rt.get_children()
	for child in children:
		var body = child as RefFrameNode
		if body == null:
			continue
		if body == PhysicsManager.camera:
			continue
			body = body.root_most_body()
		var cl_name: String = body.get_class()
		if (cl_name == "PhysicsBodyBase") or (cl_name == "Part"):
			if not (body in bodies):
				bodies.push_back( body )
	
	return bodies












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
	# Just in case if camera is parented to this rf directly.
	on_delete_rescue_camera()
	finit_physics()
	
	.on_delete()





func on_delete_rescue_camera():
	var cam: RefFrameNode = PhysicsManager.camera
	if not is_instance_valid( cam ):
		return
	
	var p: Node = cam.get_parent()
	if p != self:
		return
	
	# This node is being destroyed. If camera is parented to this node, 
	# parent it to the parent of this node.
	p = self.get_parent()
	cam.change_parent( p )




func is_orbiting():
	var t: String = motion.movement_type()
	var ret: bool = (t != "idle")
	return ret



func _set_allow_orbiting( en: bool ):
	if motion == null:
		return
	var prev: bool = motion.allow_orbiting
	if prev and (not en):
		var iii: int = 0
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
	var ass: PhysicsBodyBase = root_node.find_node( "Construction", true, false )
	DDD.important()
	DDD.print( "                All relative to assembly:" )
	for b in bodies:
		var body = b as RefFrameNode
		if body == null:
			continue
		var se3: Se3Ref = body.relative_to( ass )
		DDD.print( body.name + ": " + str(se3.r) )
	pass




