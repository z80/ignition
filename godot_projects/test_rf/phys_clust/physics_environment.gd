extends Node
class_name PhysicsEnvironment

var physics_manager_ = null
# Bit for physics contacts.
var contact_layer_: int
var bodies_: Array

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

func init( physics_manager, layer: int = 0 ):
	physics_manager_ = physics_manager
	contact_layer_   = layer


func insert_body( body ):
	var d: Dictionary
	var pb = body.create_physical()
	d["body"] = body
	d["phys"] = pb
	bodies_.push_back( d )

func cleanup():
	pass





