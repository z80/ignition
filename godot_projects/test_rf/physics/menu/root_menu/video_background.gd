extends Control


var object = null
var method: String = ""


# Called when the node enters the scene tree for the first time.
func _ready():
	play_intro()


func play_intro():
	$Intro.visible = true
	$Intro.play()
	$Loop.visible = false
	$Loop.stop()
	$Click.visible = false
	$Click.stop()


func play_loop():
	$Intro.visible = false
	$Intro.stop()
	$Loop.visible = true
	$Loop.play()
	$Click.visible = false
	$Click.stop()
	
func play_click():
	$Intro.visible = false
	$Intro.stop()
	$Loop.visible = false
	$Loop.stop()
	$Click.visible = true
	$Click.play()


func _on_Loop_finished():
	$Loop.play()





func _on_Intro_finished():
	play_loop()




func _on_Click_finished():
	if is_instance_valid( object ) and (method != ""):
		object.call( method )
