
extends Spatial
class_name ExhaustNode


enum ExhaustType {
	A = 0, 
	B = 1, 
	C = 2
}

export(ExhaustType) var exhaust_type = ExhaustType.A
export(float) var exhaust_radius = 1.0
export(float) var exhaust_length = 10.0


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
