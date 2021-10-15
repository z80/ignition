
extends RigidBody
class_name CharacterPhysicalBase


var body: Body = null



func _init():
	pass


func _ready():
	pass


func _integrate_forces( state ):
	if body == null:
		return
	body.integrate_forces( self, state )
	








