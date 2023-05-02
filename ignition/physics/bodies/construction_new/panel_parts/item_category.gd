
extends TextureButton

@export var category: Resource = null: set = _set_category

signal category_picked( cat )


func _set_category( c: Resource ):
	category = c
	
	var icon: Resource    = category.icon
	self.texture_normal   = icon
	self.texture_pressed  = icon
	self.texture_hover    = icon
	self.texture_disabled = icon
	self.texture_focused  = icon
	
	var hint: String  = category.category_visual
	self.tooltip_text = hint


# Called when the node enters the scene tree for the first time.
func _ready():
	pass




func _on_ItemButton_pressed():
	emit_signal( "category_picked", category )
