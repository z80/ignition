extends Node


var transform: Transform setget set_transform
var text: String setget set_text

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func set_transform( t ):
	$Mesh.transform = t
	var v: Viewport = get_viewport()
	if v == null:
		return
	var cam: Camera = v.get_camera()
	
	var uv: Vector2 = cam.unproject_position( t.origin )
	$Label.rect_position = uv


func set_text( stri ):
	$Label.text = stri

