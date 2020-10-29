extends Node2D


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	$Item01.connect( "click", $Background, "play_click" )
	$Item02.connect( "click", $Background, "play_click" )
	$Item02.connect( "click", $Background, "play_click" )


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_timeout():
	$Item01.start()
	$Item02.start()
	$Item03.start()




