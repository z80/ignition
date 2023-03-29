extends Node


onready var pool: ExecPool = get_node( "ExecPool" )

# Called when the node enters the scene tree for the first time.
func _ready():
	pool.start_with_arg( self, "_task", "_on_finished", 1 )
	pool.start_with_arg( self, "_task", "_on_finished", 2 )
	pool.start_with_arg( self, "_task", "_on_finished", 3 )


func _task( arg: int ):
	print( "entered ", arg )
	for i in range(10):
		OS.delay_msec( 200 )
		print( "task: ", arg, "; loop counter: ", i )
	print( "left ", arg )
	
	return arg * 100

func _on_finished( arg: int ):
	print( "finished, result: ", arg )


func _on_timer():
	pool._print_stats()

