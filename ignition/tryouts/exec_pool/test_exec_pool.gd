extends Node


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	for i in range( 3 ):
		WorkersPool.push_back( self, "task_a", "task_a_finalize" )


func task_a():
	for i in range(30):
		OS.delay_msec( 100 )
	
	print( "task_a exit" )
	return true

func task_a_finalize( ret ):
	print( "task_a_finalized: ", ret )
