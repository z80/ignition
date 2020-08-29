extends Control


signal start;
signal stop;

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Start_pressed():
	emit_signal( "start" )
	$Panel/Rec.visible = true


func _on_Stop_pressed():
	emit_signal( "stop" )
	$Panel/Rec.visible = false
