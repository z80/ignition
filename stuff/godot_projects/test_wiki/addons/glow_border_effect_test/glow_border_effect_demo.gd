extends Node


# Called when the node enters the scene tree for the first time.
func _ready():
	get_viewport().connect("size_changed", self, "resized")
	$GlowBorderEffectRenderer.resize()


func resized():
	$GlowBorderEffectRenderer.resize()
