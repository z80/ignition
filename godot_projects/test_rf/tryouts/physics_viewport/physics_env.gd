extends ViewportContainer


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func add_physics_body( body: RigidBody ):
	var s = get_node( "Vp/Scene" )
	s.add_physics_body( body )
