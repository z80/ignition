extends Control

signal value_changed( sz, res )

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_value_changed():
	var sz: float  = get_node( "Panel/Size" ).value
	var res: float = get_node( "Panel/Resolution" ).value
	emit_signal( "value_changed", sz, res )


func _on_value_changed_num(value):
	var sz: float  = get_node( "Panel/Size" ).value
	var res: float = get_node( "Panel/Resolution" ).value
	emit_signal( "value_changed", sz, res )
