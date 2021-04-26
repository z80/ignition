
extends Body


func init():
	var Visual   = load( "res://physics/bodies/parts/part_cylinder/part_cylinder_visual.tscn")
	var Physical = load( "res://physics/bodies/parts/part_cylinder/part_cylinder_physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()



func set_collision_layer( layer ):
	if _physical:
		var rb = _physical
		rb.set_collision_layer( layer )



func create_physical():
	var ph = .create_physical()



func activate():
	.activate()
	if _physical != null:
		_physical.mode = RigidBody.MODE_RIGID
		_physical.sleeping = false


func deactivate():
	.deactivate()
	if _physical != null:
		_physical.mode = RigidBody.MODE_KINEMATIC


