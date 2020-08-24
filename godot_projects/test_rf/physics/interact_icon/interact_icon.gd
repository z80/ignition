extends Control

var body: Node setget set_body, get_body
# Can override default gui showing up when mouse is hovered.
var HoverGui = null setget set_HoverGui, get_HoverGui
var ClickGui = null setget set_ClickGui, get_ClickGui


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass




func set_body( node: Node ):
	body = node


func get_body():
	return body


func set_HoverGui( gui ):
	HoverGui = gui


func get_HoverGui():
	return HoverGui


func set_ClickGui( gui ):
	ClickGui = gui


func get_ClickGui():
	return ClickGui

