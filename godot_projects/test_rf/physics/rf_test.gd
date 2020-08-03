
extends Node




# Called when the node enters the scene tree for the first time.
func _ready():
	var rf = $Rf
	BodyCreator.root_node = self
	rf.create_body( "plane" )
	rf.create_body( "cube" )
	rf.init_physics()
	
	#var v = preload( "res://physics/bodies/cube/visual.tscn" ).instance()
	#get_tree().get_root().add_child( v )
	#self.add_child( v )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process( delta ):
	var group: String = Body.GROUP_NAME
	for body in get_tree().get_nodes_in_group( group ):
		body.update_visual()
