extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var w = Input.is_action_pressed( "ui_w" )
	var s = Input.is_action_pressed( "ui_s" )
	var a = Input.is_action_pressed( "ui_a" )
	var d = Input.is_action_pressed( "ui_d" )
	if w or s or a or d:
		$AnimationTree.set( "parameters/blend_position", Vector2( 0.8, 0.0 ) )
	else:
		$AnimationTree.set( "parameters/blend_position", Vector2( -0.8, 0.0 ) )
