extends Control


var to_do: String = ""


# Called when the node enters the scene tree for the first time.
func _ready():
	$Node2D/Item01.connect( "clicked", $Background, "play_click" )
	$Node2D/Item01.connect( "clicked", self,        "_on_quit_click" )
	
	$Node2D/Item02.connect( "clicked", $Background, "play_click" )
	$Node2D/Item02.connect( "clicked", self,        "_on_start_click" )


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_start_click():
	$Background.object = self
	$Background.method = "on_start"


func _on_quit_click():
	$Background.object = self
	$Background.method = "on_quit"


func on_start():
	var t = get_tree()
	t.change_scene( "res://physics/rf_test.tscn" )



func on_quit():
	get_tree().quit()



func _on_Timer_timeout():
	$Node2D/Item01.start()
	$Node2D/Item02.start()
	$Node2D/Title.start()
