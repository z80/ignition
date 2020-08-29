
extends RefFrame
class_name RefFramePhysics


# Bit for physics contacts.
var _contact_layer: int = -1


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
	self.set_jump_t( t )
	self.apply_jump()
	var bodies = child_bodies()
	for body in bodies:
		body.update_physical_state_from_rf()




# ********************************************
#     Objects management, clustering.
# ********************************************

func jump_if_needed():
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





func child_bodies():
	var children = get_children()
	var bodies = []
	for ch in children:
		var b = ch as Body
		if b != null:
			bodies.push_back( b )
	
	return bodies












func _destructor():
	pass





