
extends CharacterVisualBase


func init():
	.init()


# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()


func set_speed_normalized( _s: float, v: Vector3 ):
	v.x = clamp( v.x, -1.0, 1.0 )
	v.z = clamp( v.z, -1.0, 1.0 )
	var at: AnimationTree = get_node( "AnimationTree" )
	at.set( "parameters/blend_position", Vector2( v.x, -v.z ) )

