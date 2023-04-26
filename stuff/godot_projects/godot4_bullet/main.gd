extends Node3D


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _physics_process(delta):
	var body: RigidBody3D = get_node( "RigidBody3D" )
	body.apply_central_force( Vector3(0.0, -1.0, 0.0) )
