
extends RefFrameNonInertialNode
class_name RefFramePhysics

@export var PhysicsEnv: PackedScene = null
var _physics_env = null
var _collision_surface: Node = null

var Clustering = preload( "res://physics/ref_frames/clustering.gd" )

# Octree borad phase for mesh queries.
var _broad_tree: BroadTreeGd = null


# For debugging jump only this number of times.
#var _jumps_left: int = 50

var debug_has_split: bool = false


# Override get_class() method in order
# to be able to identify objects of this class.
#func get_class():
#	return "RefFramePhysics"


func get_collision_surface():
	if _collision_surface == null:
		_collision_surface = get_node( "CollisionSurface" )
	
	return _collision_surface


func clone_collision_surface( other_ref_frame: RefFrameNonInertialNode ):
	var own_collision_surface: Node = get_collision_surface()
	var other_collision_surface: Node = other_ref_frame.get_collision_surface()
	own_collision_surface.clone_surface( other_collision_surface )


func _init():
	_broad_tree = BroadTreeGd.new()


func _ready():
	add_to_group( Constants.REF_FRAME_PHYSICS_GROUP_NAME )
	_create_physics_environment()


func _enter_tree():
	pass


func get_broad_tree():
	return _broad_tree


func process_children_space():
	include_close_enough_bodies()
	var has_split: bool = split_if_needed_space()
	debug_has_split = has_split
	if has_split:
		return true
	#print( "******************** merge if needed" )
	if ( merge_if_needed_space() ):
		return true
	
	#print( "******************** self delete if unused" )
	if ( self_delete_if_unused() ):
		return true
	
	return false



func process_children_surface():
	#print( "******************** apply forces" )
	#if not debug_has_split:
	#exclude_too_far_bodies()
	#print( "******************** include close enough bodies" )
	include_close_enough_bodies()
	#print( "******************** split if needed" )
	var has_split: bool = split_if_needed_surface()
	debug_has_split = has_split
	if has_split:
		return true
	#print( "******************** merge if needed" )
	# Merge on surface uses pairs.
	#if ( merge_if_needed_surface() ):
	#	return true
	
	#print( "******************** self delete if unused" )
	if ( self_delete_if_unused() ):
		return true
	
	return false






func update():
	jump_if_needed()
	
	var ok: bool = _broad_tree.subdivide( self )
	if not ok:
		print( "ERROR: failed to subdivide BroadTreeGd" )



func _change_parent( node, recursive ):
	#super( node, recursive )
	pass


# Override ready. Added surface provider creation.
#func ready():
#	_create_physics_environment()
#	
#	add_to_group( Constants.REF_FRAME_PHYSICS_GROUP_NAME )


func _create_physics_environment():
	if (_physics_env != null) and is_instance_valid(_physics_env):
		return
	var env = PhysicsEnv.instantiate()
	var root: Node = RootScene.get_root_for_physics_envs()
	root.add_child( env )
	_physics_env = env
	_physics_env.set_ref_frame( self )



func get_physics_environment():
	return _physics_env



func _destroy_physics_environment():
	if _physics_env != null:
		_physics_env.queue_free()
		_physics_env = null


# This one is called by Bodies in order to enable physics. 
func add_physics_body( body: RigidBody3D ):
	if _physics_env == null:
		_create_physics_environment()
	
	# It should exist by now. If it doesn't, I want it to fail right here.
	_physics_env.add_physics_body( body )









# If physics is initialized and being processed.
func is_active():
	var en: bool = (_physics_env != null)
	return en












func jump( t: Transform3D, v: Vector3=Vector3.ZERO ):
	# Debug output.
	var movement_type: String = self.movement_type()
	var dbg: bool = false
	self.debug = dbg
	if dbg:
		DDD.important()
		DDD.print( "jump with debug output:" )
	
#	var elliptic_motion: bool = (movement_type == "elliptic")
#	if elliptic_motion:
#		var n: RefFrameNode = get_node( "/root/Root/Sun/Planet" )
#		var se3: Se3Ref = self.relative_to( n )
#		DDD.important()
#		DDD.print( "relative_to_planet before jump: " + str(se3.r) + ", v: " + str(se3.v) )
	
	t.basis = Basis.IDENTITY
	var se3: Se3Ref = Se3Ref.new()
	se3.transform = t
	var orbiting: bool = is_orbiting()
	if not orbiting:
		v = Vector3.ZERO
	se3.v         = v
	self.jump_to( self, se3 )
	
#	if elliptic_motion:
#		var n: RefFrameNode = get_node( "/root/Root/Sun/Planet" )
#		se3 = self.relative_to( n )
#		DDD.important()
#		DDD.print( "relative_to_planet after jump: " + str(se3.r) + ", v: " + str(se3.v) )

	# Update SE3 in orbital motion.
	
#	if elliptic_motion:
#		DDD.print( "motion type after assigning se3 to motion: " + self.movement_type() )
#		se3 = self.get_se3()
#		DDD.print( "se3 right after assigning se3 to motion: " + str(se3.r) + " v: " + str(se3.v) )


	# Turn debug off and keep with debug output.
	self.debug = false
	
#	if tp == "idle":
#		return
	se3 = self.get_se3()
	var r: Vector3 = se3.r
	v = se3.v
	var l: float = self.specific_angular_momentum()
	DDD.print( "movement type: " + str(movement_type) )
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
	# But not assemblies.
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
			v += b.v()
		if qty > 0:
			v /= float( qty )
	
	var dist: float = r.length()
	if dist < Constants.RF_JUMP_DISTANCE:
		return
	
	var t: Transform3D = Transform3D.IDENTITY
	t.origin = r
	jump( t, v )
	pass




# Actually, with modern approach it shouldn't exclude.
# It should split ref frame into two. So this one is probably not used now...
func exclude_too_far_bodies():
	var max_dist: float = Constants.BODY_EXCLUDE_DIST
	var bodies: Array = root_most_child_bodies()
	var root: RefFrameRoot = get_ref_frame_root()
	var player_control = root.player_control
	var pt: Node = self.get_parent()
	
	for body in bodies:
		if body == player_control:
			continue
		
		var r: Vector3 = body.r()
		var d: float = r.length()
		if d > max_dist:
			body.change_parent( pt, false )



func include_close_enough_bodies():
	var min_dist: float = Constants.BODY_INCLUDE_DIST
	var bodies: Array = parent_bodies()
	
	for body in bodies:
		# This is to make sure that bodies added as a part of a super body 
		# are not added twice.
		var p = body.get_parent()
		if p == self:
			continue
		var se3: Se3Ref= body.relative_to( self )
		var r: Vector3 = se3.r
		var d: float = r.length()
		
		if d < min_dist:
			body.change_parent( self, false )



func print_all_ref_frames():
	DDD.important()
	DDD.print( "All ref frames" )
	var root: RefFrameRoot = self.get_ref_frame_root()
	var player_rf: RefFrameNode = root.get_player_ref_frame()
	if (player_rf != null) and ( is_instance_valid(player_rf) ):
		DDD.print( "player rf: " + player_rf.name )
	var rfs: Array = root.physics_ref_frames()
	for rf in rfs:
		var se3: Se3Ref = rf.get_se3()
		DDD.important()
		DDD.print( "rf name: " + rf.name + " r: " + str(se3.r) )
		DDD.print( "bodies: " )
		var bodies: Array = rf.child_bodies()
		for body in bodies:
			var name: String = body.name
			se3 = body.get_se3()
			DDD.print( name + ": " + str(se3.r) )


func split_if_needed_space() -> bool:
	var bodies: Array = root_most_child_bodies()
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
	DDD.print( "just before split: " )
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
	var p: Node = get_parent()
	var root: RefFrameRoot = get_ref_frame_root()
	var rf: RefFramePhysics = root.create_ref_frame_physics()
	rf.change_parent( p, false )
#	var orig_se3: Se3Ref = self.get_se3()
	
	# Compute centers of the two clusters.
	# RF will be shifter to the center of the cluster.
	# If not, it will merge again at the next iteration.
#	var se3_a: Se3Ref = Se3Ref.new()
#	var accum_a: Vector3 = Vector3.ZERO
#	for a in bodies_a:
#		var se3: Se3Ref = a.get_se3()
#		accum_a += se3.r
#	accum_a /= float( bodies_a.size() )
#	se3_a.r = accum_a
#	# Jump to the new place.
#	self.jump_to( self, se3_a )
#
#	var se3_b: Se3Ref = Se3Ref.new()
#	var accum_b: Vector3 = Vector3.ZERO
#	for b in bodies_b:
#		var se3: Se3Ref = b.get_se3()
#		accum_b += se3.r
#	accum_b /= float( bodies_b.size() )
#	se3_b.r = accum_b
#	# State in parent.
#	se3_b = orig_se3.mul( se3_b )
#	# Assign SE3.
#	rf.set_se3( se3_b )
	
	#rf.call_deferred( "clone_collision_surface", self )
	
	for body in bodies_b:
		body.call_deferred( "change_parent", rf, false )
	
	DDD.important()
	DDD.print( "new rf created " + rf. name )
	DDD.print( "after split: " )
	print_all_ref_frames()
	
	#For debugging compute distance as if we wanted to merge ref frames.
	#var dist_2: float = distance( rf )
	#_debug_distances( bodies )
	#dist_2 = distance( rf )
	
	return true 


func split_if_needed_surface() -> bool:
	var collision_surf: Node = get_collision_surface()
	var running: bool = collision_surf.is_running()
	if running:
		return false
	
	var node_clusters: Array = collision_surf.get_bounding_node_clusters()
	var qty: int = node_clusters.size()
	if qty < 2:
		return false
	
	var bodies: Array = root_most_child_bodies()
	if ( bodies.size() < 2 ):
		return false
	
	var last_cluster: Array = node_clusters.front()
	var cells_in_last_cluster_qty: int = last_cluster.size()
	
	var rot: RefFrameRotationNode = get_parent()
	var surface: MarchingCubesDualGd = collision_surf.get_surface()
	
	var bodies_b: Array = []
	
	qty = bodies.size()
	for i in range(qty):
		var body: RefFrameBodyNode = bodies[i]
		var se3: Se3Ref = body.relative_to( rot )
		var inside: bool = BoundingNodeGd.cluster_contains_point( surface, last_cluster, se3 )
		if inside:
			bodies_b.push_back( body )
	
	var root: RefFrameRoot = get_ref_frame_root()
	var rf: RefFramePhysics = root.create_ref_frame_physics()
	rf.change_parent( rot, false )
	var current_rf_se3: Se3Ref = self.get_se3()
	rf.set_se3( current_rf_se3 )
	
	call_deferred( "_post_split_surface", rf, last_cluster, bodies_b )
	
	return true


func _post_split_surface( new_rf: RefFrameNode, nodes: Array, bodies: Array ):
	var surf: Node = get_collision_surface()
	surf.move_cells_to_other_ref_frame( nodes, new_rf )
	for body in bodies:
		body.change_parent( new_rf, false )



func merge_if_needed_space():
	var root: RefFrameRoot = get_ref_frame_root()
	var ref_frames: Array = root.physics_ref_frames()
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
			
			var bodies: Array = rf.root_most_child_bodies()
			for body in bodies:
				body.change_parent( self )
			
			
			DDD.print( "merged " + rf.name + " with " + self.name )
			# Also check if it is player's ref frame.
			# If it is, change it to the one everything is merged to.
			var player_rf: RefFramePhysics = root.get_player_ref_frame()
			if rf == player_rf:
				DDD.print( "player ref frame changed to " + rf.name )
			
			# Queue for deletion.
			rf.queue_free()
			
			DDD.print( "info after" )
			print_all_ref_frames()
			
			return true
	
	return false



func merge_if_needed_surface( other_rf: RefFramePhysics ):
	var collision_surf_other: Node  = other_rf.get_collision_surface()
	var running: bool = collision_surf_other.is_running()
	if running:
		return false
	
	var bounding_nodes_other: Array = collision_surf_other.get_bounding_nodes()

	var collision_surf_own: Node  = get_collision_surface()
	running = collision_surf_own.is_running()
	if running:
		return false
	
	var clusters: Array = collision_surf_own.get_bounding_node_clusters( bounding_nodes_other )
	
	var clusters_qty: int = clusters.size()
	if clusters_qty > 1:
		return false
	
	# Bounding node clusters touch each other. So we merge the other ref. frame into this one.
	var bodies: Array = other_rf.root_most_child_bodies()
	for body in bodies:
		body.change_parent( self, false )
	
	# Here the origins of individual cells of other_rf are different compared to 
	# the current ref. frame. Besides of just reparenting eed to relocate them.
	# Or may be not because they are RefFrameNode derivatives which is supposed 
	# to do relocation automatically.
	var surf_other: Node = other_rf.get_collision_surface()
	surf_other.move_cells_to_other_ref_frame( bounding_nodes_other, self )
	
	# Queue the other ref. frame for deletion.
	other_rf.queue_free()
	
	return true



# Need to be removed if returned "true".
func self_delete_if_unused():
	
	# Also don't delete player ref. frame.
	var player_rf: RefFramePhysics = RootScene.ref_frame_root.get_player_ref_frame()
	if self == player_rf:
		return false
	
	# And don't delete if parenting the camera.
	var cam: RefFrameNode = RootScene.ref_frame_root.player_camera
	if is_instance_valid( cam ):
		var p: Node = cam.get_parent()
		if p == self:
			# In this case may be at most stop orbiting to not 
			# fall inside if a planet
			return false
		
		elif p != null:
			p = p.get_parent()
			if p == self:
				return false
	
	
	# Check all
	var bodies: Array = root_most_child_bodies()
	var qty: int = bodies.size()
	
	# If there are no bodies, delete it.
	if ( qty < 1 ):
		on_delete()
		self.queue_free()
		return true
	
	# Also for non-orbiting ref. frames delete it if all 
	# objects are stationary.
	var orbiting: bool = is_orbiting()
	# Don't delete orbiting ref. frames.
	if not orbiting:
		var moving_qty: int = 0
		for i in range(qty):
			var b: RefFrameBodyNode = bodies[i]
			var se3: Se3Ref = b.get_se3()
			var v: float = se3.v.length()
			var w: float = se3.w.length()
			v = max(v, w)
			if v > Constants.IDLE_SPEED_THRESHOLD:
				moving_qty += 1
		
		if moving_qty == 0:
			on_delete()
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
	var children: Array = get_children()
	var bodies: Array = []
	for ch in children:
		var b = ch as RefFrameBodyNode
		var a = ch as RefFrameAssemblyNode
		var include: bool = (b != null) and (a == null)
		if include:
			bodies.push_back( b )
	
	return bodies


func root_most_child_bodies():
	var children = get_children()
	var bodies = []
	for ch in children:
		var b: RefFrameNode = ch as RefFrameBodyNode
		if b == null:
			continue
		
		if (b != null):
			var root_most_body: RefFrameNode = b #.root_most_body()
			var append: bool = not (root_most_body in bodies)
			if append:
				bodies.push_back( root_most_body )
	
	return bodies


func parent_bodies():
	var pt: Node = self.get_parent()
	var rt: RefFrameNode = RootScene.ref_frame_root
	
	var bodies = []
	
	var children_qty: int = pt.get_child_count()
	for ind in range(children_qty):
		var child: Node = pt.get_child( ind )
		var body = child as RefFrameBodyNode
		if body == null:
			continue
		
		body = body.root_most_body()
		if not (body in bodies):
			bodies.push_back( body )
	
	if rt == pt:
		return bodies

#	children = rt.get_children()
#	for child in children:
#		var body = child as RefFrameNode
#		if body == null:
#			continue
#		if body == RootScene.ref_frame_root.player_camera:
#			continue
#		body = body.root_most_body()
#		var cl_name: String = body.get_class()
#		if (cl_name == "PhysicsBodyBase") or (cl_name == "Part"):
#			if not (body in bodies):
#				bodies.push_back( body )
	
	return bodies












func distance( b: RefFramePhysics ):
	var bodies_a: Array = root_most_child_bodies()
	var bodies_b: Array = b.root_most_child_bodies()
	if bodies_a.is_empty() or bodies_b.is_empty():
		return 2.0 * Constants.RF_SPLIT_DISTANCE
	
	#var bodies_all: Array = bodies_a + bodies_b
	#_debug_distances( bodies_all )
	
	var min_d: float = -1.0
	for body_a in bodies_a:
		for body_b in bodies_b:
			var d: float = body_a.distance( body_b )
			if (min_d < 0.0) or (d < min_d):
				min_d = d
	
	return min_d



func _exit_tree():
	var to_be_deleted: bool = is_queued_for_deletion()
	if to_be_deleted:
		on_delete()



func on_delete():
	_re_parent_children_on_delete()
	# Just in case if camera is parented to this rf directly.
	_on_delete_rescue_camera()
	_destroy_physics_environment()
	





func _on_delete_rescue_camera():
	var cam: RefFrameNode  = RootScene.ref_frame_root.player_camera
	if not is_instance_valid( cam ):
		return
	
	var p: Node = cam.get_parent()
	if p != self:
		return
	
	# This node is being destroyed. If camera is parented to this node, 
	# parent it to the parent of this node.
	p = self.get_parent()
	cam.change_parent( p, false )



func _re_parent_children_on_delete():
	var orbiting: bool = is_orbiting()
	# If orbiting, don't re-parent children.
	# There probably is a reason for this deletion.
	if orbiting:
		return
	
	var p: Node = get_parent()
	if (p == null) or (not is_instance_valid(p)):
		return
	
	var children: Array = root_most_child_bodies()
	for ch in children:
		if (ch != null) and is_instance_valid(ch):
			ch.change_parent( p )



func _serialize( data: Dictionary ):
	pass



func _deserialize( data: Dictionary ):
	return true



#static func _debug_distances( bodies: Array ):
#	var bodies_node: Node = RootScene.get_root_for_bodies()
#	var tree: SceneTree = bodies_node.get_tree()
#	var vp: SubViewport = tree.root
#	var root_node: Node = vp.get_node( "Root" ).get_node( "Sun" )
#	var ass: PhysicsBodyBase = root_node.find_child( "Construction", true, false )
#	DDD.important()
#	DDD.print( "                All relative to assembly:" )
#	for b in bodies:
#		var body = b as RefFrameNode
#		if body == null:
#			continue
#		var se3: Se3Ref = body.relative_to( ass )
#		DDD.print( body.name + ": " + str(se3.r) )
#	pass



# Closest not in terms of distance. But in terms of graph node distance.
func closest_parent_ref_frame():
	var p = self.get_parent()
	if p == null:
		return null
	
	var rf: RefFrame = _closest_parent_ref_frame_recursive( p )
	return rf
	
	
func _closest_parent_ref_frame_recursive( n: Node ):
	if n == null:
		return null
	
	var rf: RefFrame = n as RefFrame
	if rf:
		return rf
	
	var p: Node = n.get_parent()
	return _closest_parent_ref_frame_recursive( p )
	

func closest_force_source():
	var n: Node =  _force_source_recursive( self )
	if n == null:
		return null
	var fs: RefFrame = n as RefFrame
	return fs


func _force_source_recursive( n: Node ):
	if n == null:
		return null
	
	var rf: RefFrame = n as RefFrame
	if rf != null:
		if rf.force_source != null:
			return rf
	
	var p: Node = n.get_parent()
	return _force_source_recursive( p )


func get_ref_frame_root():
	var rf: RefFrameNode = RootScene.ref_frame_root
	return rf




func register_in_node( body: RefFrameBodyNode ):
	var rot: RefFrameRotationNode = get_parent()
	if rot == null:
		return false

	var collision_surf: Node = get_collision_surface()
	var surface: MarchingCubesDualGd = collision_surf.gt_surface()
	var node_cluster: Array = collision_surf.get_bounding_nodes()
	
	var se3: Se3Ref = body.relative_to( rot )
	var qty: int = node_cluster.size()
	for i in range(qty):
		var node: BoundingNodeGd = node_cluster[i]
		var inside: bool = node.contains_point( surface, se3 )
		if not inside:
			continue
		
		# Bind the point and the volume node.
	
	return false


