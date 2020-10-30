extends Control


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	$Node2D/Item01.connect( "clicked", $Background, "play_click" )
	$Node2D/Item02.connect( "clicked", $Background, "play_click" )
	$Node2D/Item02.connect( "clicked", $Background, "play_click" )


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass








func _on_Timer_timeout():
	$Node2D/Item01.start()
	$Node2D/Item02.start()
	$Node2D/Item03.start()
	$Node2D/Title.start()
