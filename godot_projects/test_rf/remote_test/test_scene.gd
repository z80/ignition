extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


remotesync func set_visibles( cube, cyl ):
	$Cube.visible     = cube
	$Cylinder.visible = cyl


remotesync func move_to( v: Vector3 ):
	self.position = v
