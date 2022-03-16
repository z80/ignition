
extends TextureButton

export(Resource) var block_desc = null setget _set_block_desc

signal block_picked( desc )


func _set_block_desc( c: Resource ):
	block_desc = c
	
	var icon: Resource    = block_desc.icon
	self.texture_normal   = icon
	self.texture_pressed  = icon
	self.texture_hover    = icon
	self.texture_disabled = icon
	self.texture_focused  = icon
	
	var hint: String = block_desc.category_visual
	self.hint = hint


# Called when the node enters the scene tree for the first time.
func _ready():
	pass




func _on_ItemButton_pressed():
	emit_signal( "block_picked", block_desc )
