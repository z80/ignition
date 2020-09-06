extends Control


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Frames_pressed():
	var frames = find_node( "ContainerFrames" )
	var parts = find_node( "ContainerParts" )
	frames.visible = true
	parts.visible  = false


func _on_Parts_pressed():
	var frames = find_node( "ContainerFrames" )
	var parts = find_node( "ContainerParts" )
	frames.visible = false
	parts.visible  = true
