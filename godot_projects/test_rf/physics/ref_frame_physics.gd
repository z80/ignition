
extends RefFrame
class_name RefFramePhysics


var _physics_env = null
var _bodies: Array = []
# Bit for physics contacts.
var _contact_layer: int = -1


func init_physics():
	if _contact_layer >= 0:
		return true
	
	_contact_layer = PhysicsManager.acquire_environment( self )
	if ( _contact_layer < 0 ):
		return false
	
	for body in _bodies:
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


func create_body( type_name: String, t: Transform = Transform.IDENTITY ):
	var body = BodyCreator.create( type_name )
	add_child( body )
	body.set_t( t )
	
	_bodies.push_back( body )
	if ( _contact_layer >= 0 ):
		var ph = body.create_physical()
		ph.set_contact_layer( _contact_layer )


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
	for body in _bodies:
		body.remove_physical()





func _destructor():
	pass





