extends Panel

signal launch
signal abort


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Launch_pressed():
	emit_signal( "launch" )


func _on_Abort_pressed():
	emit_signal( "abort" )
