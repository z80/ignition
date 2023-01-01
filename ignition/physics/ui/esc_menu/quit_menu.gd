
extends MenuBase



var _menu_stack: Array = []





func _on_Ok_pressed():
	var _err_code: int = OS.shell_open( 'https://litedictteam.itch.io/ignition' )
	
	_on_Quit_pressed()


func _on_Quit_pressed():
	var t: SceneTree = get_tree()
	t.quit()
	UiSound.play( Constants.ButtonClick )

