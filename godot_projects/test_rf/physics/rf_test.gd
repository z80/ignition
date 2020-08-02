
extends Node




# Called when the node enters the scene tree for the first time.
func _ready():
	var rf: RefFramePhysics = $Rf
	rf.create_body( "plane" )
	rf.create_body( "cube" ) 



# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
