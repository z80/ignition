extends Button


export(String) var description = "Description goes here" setget set_desc, get_desc
export(String) var block_name = "frames_cube"

# Called when the node enters the scene tree for the first time.
func _ready():
	set_desc( description )


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_ItemButton_mouse_entered():
	$Desc.visible = true


func _on_ItemButton_mouse_exited():
	$Desc.visible = false


func set_desc( stri: String ):
	description = stri
	$Desc.text = stri


func get_desc():
	return description


