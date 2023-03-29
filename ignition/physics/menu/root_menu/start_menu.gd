extends Control

export(PackedScene) var GameScene = null

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
	t.change_scene( GameScene.resource_path )



func on_quit():
	get_tree().quit()



func _on_Timer_timeout():
	$Node2D/Item01.start()
	$Node2D/Item02.start()
	$Node2D/Title.start()


func _on_url_meta_clicked(meta):
	var path: String = str( meta )
	var _err_code: int = OS.shell_open( path )


func _on_help_pressed():
	var Wiki = preload( "res://wiki_handler/wiki_window.tscn" )
	var wiki: Control = Wiki.instance()
	add_child( wiki )

