
extends CharacterVisualBase

@onready var _animation_tree: AnimationTree = $AnimationTree


func init():
	super.init()


# Called when the node enters the scene tree for the first time.
func _ready():
	target_tps = get_node("target_tps")
	target_fps = get_node("target_fps")
	set_speed_normalized( 0.0, Vector3.ZERO )
	
	_animation_tree = get_node( "AnimationTree" )


func set_speed_normalized( _s: float, v: Vector3 ):
	v.x = clamp( v.x, -1.0, 1.0 )
	v.z = clamp( v.z, -1.0, 1.0 )
	var anim_at: Vector2 = Vector2( v.x, -v.z )
	_animation_tree.set( "parameters/blend_position", anim_at )
	
	#print( "anim: ", anim_at )



func _process( delta ):
	return
	var at: AnimationTree = get_node( "AnimationTree" )
	at.advance( delta )
	var ap: AnimationPlayer = get_node("astronaut/AnimationPlayer")
	ap.advance( delta )



func _on_animation_tree_animation_finished(anim_name):
	print( "animation finished: ", anim_name )


func _on_animation_tree_animation_started(anim_name):
	print( "animation started: ", anim_name )
