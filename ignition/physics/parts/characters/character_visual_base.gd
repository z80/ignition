
extends Node3D
class_name CharacterVisualBase

var target_tps: Node3D = null
var target_fps: Node3D = null
var body: PhysicsBodyBase = null

# Called when the node enters the scene tree for the first time.
func _ready():
	set_speed_normalized( 0.0, Vector3.ZERO )


# This one should be overridden.
func set_speed_normalized( _s: float, _v: Vector3 ):
	pass




func _integrate_forces( state ):
	if body == null:
		return
	body.integrate_forces( self, state )
	
