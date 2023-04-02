
extends RigidBody3D
class_name CharacterPhysicalBase


var body: PhysicsBodyBase = null



func _init():
	pass


func _ready():
	pass


func _integrate_forces( state ):
	if body == null:
		return
	body.integrate_forces( self, state )
	








