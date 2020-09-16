
extends RefFrame
class_name RefFramePhysics


# Bit for physics contacts.
var _contact_layer: int = -1


# For debugging jump only this number of times.
#var _jumps_left: int = 50


func process_children():
	.process_children()
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
	
	if ( _contact_layer >= 0 ):
		var ph = body.create_physical()
		if ph != null:
			ph.set_collision_layer( _contact_layer )


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
	
	var player_focus = PhysicsManager.player_focus
	var bodies = child_bodies()
	if not (player_focus in bodies):
		return
	
	var r: Vector3 = player_focus.r()
	var dist: float = r.length()
	if dist < Constants.RF_JUMP_DISTANCE:
		return
	
	var t: Transform = player_focus.t()
	jump( t )
	
	#_jumps_left -= 1



func exclude_too_far_bodies():
	var max_dist: float = Constants.BODY_EXCLUDE_DIST
	var bodies = child_bodies()
	var player_focus = PhysicsManager.player_focus
	var pt = self.get_parent()
	
	for body in bodies:
		if body == player_focus:
			continue
		
		var r: Vector3 = body.r()
		var d: float = r.length()
		if d > max_dist:
			body.remove_physical()
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





func _destructor():
	pass





