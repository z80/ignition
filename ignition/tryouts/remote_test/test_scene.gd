extends Node3D


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


@rpc("any_peer", "call_local") func set_visibles( cube, cyl ):
	$Cube.visible     = cube
	$Cylinder.visible = cyl


@rpc("any_peer", "call_local") func move_to( v: Vector3 ):
	self.position = v
