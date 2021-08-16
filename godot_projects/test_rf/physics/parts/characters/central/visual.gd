
extends CharacterVisualBase


func init():
	.init()


# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()


func set_speed_normalized( s: float, v: Vector3 ):
	s = clamp( s, 0.0, 1.0 )
	var at: AnimationTree = get_node( "AnimationTree" )
	at.set( "parameters/blend_position", s )

