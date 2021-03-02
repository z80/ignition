
extends RefFrame
class_name RefFramePhysics

var Clustering = preload( "res://physics/ref_frames/clustering.gd" )

# Bit for physics contacts.
var _contact_layer: int = -1

# Surface collision.
var SurfaceProvider = preload( "res://physics/bodies/surface_provider/surface_provider.tscn" )
var _surface_provider = null

# Subdividion source reference in order to determine 
# if it's time to rebuild the sphere visual and collision surface.
# Can collide with just one.
var _subdivide_source_physical: SubdivideSourceRef = null

# For debugging jump only this number of times.
#var _jumps_left: int = 50



# To be able to identify objects of this class.
func get_class():
	return "RefFramePhysics"


func process_children():
	.process_children()
	apply_forces()
	#exclude_too_far_bodies()
	include_close_enough_bodies()
	split_if_needed()
	merge_if_needed()
	self_delete_if_unused()




func evolve():
	.evolve()
	jump_if_needed()


# Override ready. Added surface provider creation.
func ready():
	.ready()
	create_surface_provider()
	create_subdivide_source()


func create_surface_provider():
	if _surface_provider != null:
		return
	_surface_provider = SurfaceProvider.instance()
	_surface_provider.init()
	_surface_provider.change_parent( self )


func create_subdivide_source():
	if _subdivide_source_physical != null:
		return
	
	_subdivide_source_physical = SubdivideSourceRef.new()




func set_surface_vertices( verts: PoolVector3Array, se3: Se3Ref ):
	print( "surface provides is given ", verts.size(), " vertices" )
	if _surface_provider != null:
		_surface_provider.set_vertices( verts, se3 )


func update_surface_vertices( se3: Se3Ref ):
	if _surface_provider != null:
		_surface_provider.update_vertices( se3 )


func init_physics():
	if _contact_layer >= 0:
		return true
	
	_contact_layer = PhysicsManager.acquire_environment( self )
	if ( _contact_layer < 0 ):
		return false
	
	var bodies = child_bodies()
	for body in bodies:
		var ph = body.create_physical()
		ph.collision_layer = _contact_layer



func finit_physics():
	if ( _contact_layer >= 0 ):
		_cleanup_physical()
		PhysicsManager.release_environment( self )



# If physics is initialized and being processed.
func is_active():
	var en: bool = (_contact_layer > 0)
	return en



func add_body( body: Body ):
	if body == null:
		return
	
	# Make it parented.
	body.change_parent( self )



func remove_body( body: Body ):
	var parent = self.get_parent()
	body.change_parent( parent )



func create_body( type_name: String, t: Transform = Transform.IDENTITY ):
	var body = BodyCreator.create( type_name )
	add_child( body )
	body.set_t( t )
	
	if ( _contact_layer >= 0 ):
		var ph = body.create_physical()
		ph.set_collision_layer( _contact_layer )


func _cleanup_physical():
	if _contact_layer < 0:
		return
	var bodies = child_bodies()
	for body in bodies:
		body.remove_physical()



func jump( t: Transform ):
	var before_t: Transform = self.t()
	
	t.basis = Basis.IDENTITY
	self.set_jump_t( t )
	self.apply_jump()
	var bodies = child_bodies( true )
	for body in bodies:
		body.update_physical_state_from_rf()
	
	var after_t: Transform = self.t()
	print( "RefFramePhysics jumped from ", before_t, " to ", after_t )





# ********************************************
#     Objects management, clustering.
# ********************************************

func jump_if_needed():
	# This is for debugging. Jump only this amount of times.
	#if _jumps_left <= 0:
	#	return
	
	var player_control = PhysicsManager.player_control
	var bodies = child_bodies()
	if not (player_control in bodies):
		return
	
	var r: Vector3 = player_control.r()
	var dist: float = r.length()
	if dist < Constants.RF_JUMP_DISTANCE:
		return
	
	var t: Transform = player_control.t()
	t.basis = Basis.IDENTITY
	jump( t )
	
	#_jumps_left -= 1



# Actually, with modern approach it shouldn't exclude.
# It should split ref frame into two. So this one is probably not used now...
func exclude_too_far_bodies():
	var max_dist: float = Constants.BODY_EXCLUDE_DIST
	var bodies = child_bodies()
	var player_control = PhysicsManager.player_control
	var pt = self.get_parent()
	
	for body in bodies:
		if body == player_control:
			continue
		
		var r: Vector3 = body.r()
		var d: float = r.length()
		if d > max_dist:
			#body.remove_physical()
			body.change_parent( pt )



func include_close_enough_bodies():
	var min_dist: float = Constants.BODY_INCLUDE_DIST
	var bodies = parent_bodies()
	
	for body in bodies:
		body.compute_relative_to_root( self )
		var r: Vector3 = body.r_root()
		var d: float = r.length()
		
		if d < min_dist:
			add_body( body )



func split_if_needed():
	var bodies = child_bodies()
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
	
	var bodies_a: Array = []
	var bodies_b: Array = []
	for i in range(split_ind):
		var body: Body = dest[i]
		bodies_a.push_back( body )
	for i in range( split_ind, qty ):
		var body: Body = dest[i]
		bodies_b.push_back( body )
	
	# Check on which side the user controlled body is.
	var player_control = PhysicsManager.player_control
	var player_in_b: bool = false
	for body in bodies_b:
		if body == player_control:
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
	for body in bodies_b:
		body.change_parent( rf )
	
	return true 


func merge_if_needed():
	var ref_frames: Dictionary = PhysicsManager.physics_ref_frames()
	for id in ref_frames:
		var rf: RefFramePhysics = ref_frames[id]
		if rf == self:
			continue
		var queued_for_deletion: bool = rf.is_queued_for_deletion()
		if queued_for_deletion:
			continue
		
		var dist: float = distance( rf )
		if dist < Constants.RF_MERGE_DISTANCE:
			var bodies: Array = rf.child_bodies()
			for body in bodies:
				body.change_parent( self )
			
			return true
	return false


# Need to be removed if returned "true".
func self_delete_if_unused():
	var bodies: Array = child_bodies()
	var qty: int = bodies.size()
	if ( qty < 1 ):
		# Also don't delete player ref. frame.
		var player_rf: RefFramePhysics = PhysicsManager.player_ref_frame
		if self == player_rf:
			return false
		self.queue_free()
		return true
	return false



func child_bodies( including_surf_provider: bool = false ):
	var children = get_children()
	var bodies = []
	for ch in children:
		var b = ch as Body
		var include: bool = (b != null)
		if not including_surf_provider:
			include = include and (b != _surface_provider)
		if include:
			bodies.push_back( b )
	
	return bodies



func parent_bodies():
	var pt = self.get_parent()
	var rt = BodyCreator.root_node
	
	var bodies = []
	
	var children = pt.get_children()
	for child in children:
		var body = child as Body
		if body != null:
			bodies.push_back( body )
	
	if rt == pt:
		return bodies

	children = rt.get_children()
	for child in children:
		var body = child as Body
		if body != null:
			bodies.push_back( body )
	
	return bodies



func apply_forces():
	var rf: RefFrame = closest_force_source()
	if rf == null:
		return
	
	var children = self.get_children()
	for child in children:
		var body = child as Body
		if body != null:
			process_body( rf, body )
	



func process_body( force_source_rf: RefFrame, body: Body, up_defined: bool = false ):
	force_source_rf.compute_relative_to_root( body )
	var r: Vector3 = force_source_rf.r_root()
	var v: Vector3 = force_source_rf.v_root()
	var q: Quat    = force_source_rf.q_root()
	var w: Vector3 = force_source_rf.w_root()
	
	# This "if" statement is for debugging.
	if body.name == "Thruster_01":
		body.compute_relative_to_root( force_source_rf )
		var r_rel: Vector3 = body.r_root()
		var q_rel: Quat    = body.q_root()
		var qq: Quat = q_rel * q
		var i = 1
		
	
	var ret: Array = []
	var fs: ForceSource = force_source_rf.force_source
	fs.compute_force( body, r, v, q, w, ret )
	if not up_defined:
		var defines_vertical: bool = fs.defines_vertical()
		if defines_vertical:
			up_defined = true
			var p: Node = body.get_parent()
			var rfp: RefFramePhysics = p as RefFramePhysics
			if rfp != null:
				var up: Vector3 = fs.up( force_source_rf, body )
				up = body.q().xform( up )
				body.set_local_up( up )
	
	var F: Vector3 = ret[0]
	var P: Vector3 = ret[1]
	# Convert to physics ref. frame.
	var q_adj: Quat = body.q()

	F = q_adj.xform( F )
	P = q_adj.xform( P )

	# Apply force and torque to the body.
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
	var bodies_a: Array = child_bodies()
	var bodies_b: Array = b.child_bodies()
	var min_d: float = -1.0
	for body_a in bodies_a:
		for body_b in bodies_b:
			var d: float = body_a.distance_min( body_b )
			if (min_d < 0.0) or (d < min_d):
				min_d = d
	
	return min_d


# Destructor.
func _exit_tree():
	finit_physics()





