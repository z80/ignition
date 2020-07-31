
extends RfBase
class_name Rf


var _physics_env = null
var _bodies: Array = []


func init_physics():
	if _physics_env != null:
		return
	
	_physics_env = PhysicsManager.create_environment()
	for body in _bodies:
		_physics_env.insert_body( body )
		var ph = body.create_physical()
		ph.set_contact_layer( _physics_env._contact_layer )


func finit_physics():
	if ( _physics_env ):
		_physics_env.cleanup()
		_physics_env.queue_free()


func create_body( type_name: String, t: Transform ):
	return null



