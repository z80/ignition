@tool
extends HBoxContainer

signal value_changed(value)

@export var text: String := "Slider": set = set_text
@export var min_value: float := 0.0: set = set_min_value
@export var max_value: float := 1.0: set = set_max_value
@export var value: float := 0.0: set = set_value

func _ready() -> void:
	set_text(text)
	set_min_value(min_value)
	set_max_value(max_value)
	set_value(value)


func set_text(string: String) -> void:
	text = string
	if has_node("Name"):
		$Name.text = text + ": "


func set_value(val: float) -> void:
	value = val
	if has_node("HSlider"):
		$HSlider.value = value
		$Value.text = str(snapped(value, 0.01))


func set_min_value(val: float) -> void:
	min_value = min(val, max_value)
	if has_node("HSlider"):
		$HSlider.min_value = min_value


func set_max_value(val: float) -> void:
	max_value = max(val, min_value)
	if has_node("HSlider"):
		$HSlider.max_value = max_value


func _on_HSlider_value_changed(value: float) -> void:
	emit_signal("value_changed", value)
	$Value.text = str(snapped(value, 0.01))
