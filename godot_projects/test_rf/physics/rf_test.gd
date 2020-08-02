
extends Node




# Called when the node enters the scene tree for the first time.
func _ready():
	var rf = $Rf
	rf.create_body( "plane" )
	rf.create_body( "cube" ) 
	rf.init_physics()


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process( delta ):
	var group: String = Body.GROUP_NAME
	for body in get_tree().get_nodes_in_group( group ):
		body.update_visual()
