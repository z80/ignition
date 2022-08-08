extends Node


onready var pool: ExecPool = get_node( "ExecPool" )

# Called when the node enters the scene tree for the first time.
func _ready():
	pool.start_arg( self, "_task", "_on_finished", 1 )
	pool.start_arg( self, "_task", "_on_finished", 2 )
	pool.start_arg( self, "_task", "_on_finished", 3 )


func _task( arg: int ):
	print( "entered ", arg )
	OS.delay_msec( 3000 )
	print( "left ", arg )
	
	return arg

func _on_finished( arg: int ):
	print( "finished, result: ", arg )


func _on_timer():
	pool._print_stats()

