
extends RefFrame
class_name RefFramePhysics


# Bit for physics contacts.
var _contact_layer: int = -1


# For debugging jump only this number of times.
#var _jumps_left: int = 50

# To be able to identify objects of this class.
func get_class():
	return "RefFramePhysics"


func process_children():
	.process_children()
	apply_forces()
	exclude_too_far_bodies()
	include_close_enough_bodies()




func evolve():
	.evolve()
	jump_if_needed()



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
	var en: bool = (_contact_layer >= 0)
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


func compute_relative_states():
	#_compute_state_recursive( self, root )
	var group: String = Body.GROUP_NAME
	for body in get_tree().get_nodes_in_group( group ):
		# Update visual element position with respect to this ref. frame.
		# "self" indicates that.
		body.update_visual( self )



func _cleanup_physical():
	if _contact_layer < 0:
		return
	var bodies = child_bodies()
	for body in bodies:
		body.remove_physical()



func jump( t: Transform ):
	var before_t: Transform = self.t()
	
	self.set_jump_t( t )
	self.apply_jump()
	var bodies = child_bodies()
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
	jump( t )
	
	#_jumps_left -= 1



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






func child_bodies():
	var children = get_children()
	var bodies = []
	for ch in children:
		var b = ch as Body
		if b != null:
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
	
	#var defines_vertical: bool = rf.force_source.defines_vertical()
	#if defines_vertical:
	#	rf.compute_relative_to_root( self )
	#	var q: Quat = rf.q_root()



func process_body( force_source_rf: RefFrame, body: Body ):
	force_source_rf.compute_relative_to_root( body )
	var r: Vector3 = force_source_rf.r_root()
	var v: Vector3 = force_source_rf.v_root()
	var q: Quat    = force_source_rf.q_root()
	var w: Vector3 = force_source_rf.w_root()

	var ret: Array = []
	force_source_rf.force_source.compute_force( body, r, v, q, w, ret )

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
				process_body( force_source_rf, body )




# Destructor.
func _exit_tree():
	pass





