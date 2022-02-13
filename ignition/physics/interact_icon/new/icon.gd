extends TextureButton


signal icon_clicked


export(String) var title: String = "" setget _set_title, _get_title


# Called when the node enters the scene tree for the first time.
func _ready():
#	var B: PackedScene = load( "res://physics/interact_icon/new/b.tscn" )
#	var S: Script = load( "res://physics/interact_icon/new/b.gd" )
#	var vp: String = S.get_visual_path()
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _set_title( t: String ):
	self.hint_tooltip = t


func _get_title():
	return title



func _on_pressed():
	emit_signal( "icon_clicked" )



func move_to( at: Vector2 ):
	var rect: Rect2 = self.get_global_rect()
	var sz2: Vector2 = rect.size * 0.5
	
	var to: Vector2 = at - sz2
	
	self.set_position( to )


