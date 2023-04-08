
extends CharacterVisualBase


func init():
	super.init()


# Called when the node enters the scene tree for the first time.
func _ready():
	target_tps = get_node("target_tps")
	target_fps = get_node("target_fps")
	set_speed_normalized( 0.0, Vector3.ZERO )


func set_speed_normalized( _s: float, v: Vector3 ):
	v.x = clamp( v.x, -1.0, 1.0 )
	v.z = clamp( v.z, -1.0, 1.0 )
	var at: AnimationTree = get_node( "AnimationTree" )
	var anim_at: Vector2 = Vector2( v.x, -v.z )
	#at.set( "parameters/blend_position", anim_at )
	
	#print( "anim: ", anim_at )



func _process( delta ):
	var at: AnimationTree = get_node( "AnimationTree" )
	at.advance( delta )
	var ap: AnimationPlayer = get_node("astronaut/AnimationPlayer")
	ap.advance( delta )



func _on_animation_tree_animation_finished(anim_name):
	print( "animation finished: ", anim_name )


func _on_animation_tree_animation_started(anim_name):
	print( "animation started: ", anim_name )
