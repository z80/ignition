
extends RefFrameNode
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
		_physics_env.insert_body( body )
		var ph = body.create_physical()
		ph.set_contact_layer( _contact_layer )


func finit_physics():
	if ( _contact_layer >= 0 ):
		_cleanup_physical()
		PhysicsManager.release_environment( self )


func create_body( type_name: String, t: Transform ):
	var body: Body = BodyCreator.create( type_name )
	_bodies.push_back( body )
	add_child( body )
	if ( _contact_layer >= 0 ):
		var ph = body.create_physical()
		ph.set_contact_layer( _contact_layer )


func _cleanup_physical():
	if _contact_layer < 0:
		return
	for body in _bodies:
		body.remove_physical()


func _destructor():
	pass





