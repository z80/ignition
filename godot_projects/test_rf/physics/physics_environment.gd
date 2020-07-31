extends Node
class_name PhysicsEnvironment

# Bit for physics contacts.
var _contact_layer: int
var _rf: Rf

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

func init( layer: int = 0 ):
	_contact_layer   = layer


func insert_body( body ):
	var pb = body.create_physical()
	if pb != null:
		body.set_contact_layer( _contact_layer )


func cleanup():
	if not _rf:
		return
		
	for body in _rf._bodies:
		body.remove_physical()





