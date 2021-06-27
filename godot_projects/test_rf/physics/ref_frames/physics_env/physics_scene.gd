extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func add_physics_body( body: RigidBody ):
	add_child( body )
	body.owner = self
