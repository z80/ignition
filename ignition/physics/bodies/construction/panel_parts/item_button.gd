extends Button


@export var description: String = "Description goes here": get = get_desc, set = set_desc
@export var block_name: String = "frames_cube"
@export var dynamic: bool    = true

signal create_block( name, dynamic )

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




func _on_ItemButton_pressed():
	emit_signal( "create_block", block_name, dynamic )
