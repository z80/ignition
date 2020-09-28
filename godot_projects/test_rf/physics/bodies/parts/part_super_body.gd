extends Body
class_name PartSuperBody

# This class handles dynamic body group.
# On every update it should average it's position to the mean of 
# all sub-bodies.

# Also should keep connection information in order to create/destroy joints.


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func process_inner( delta ):
	.process_inner( delta )
	# Loop over all sub bodies.
	var at: Vector3 = Vector3.ZERO
	for body in sub_bodies:
		var r: Vector3 = body.r()
		at += r
	
	var qty: int = sub_bodies.size()
	if qty > 0:
		at = at / qty
	
	self.set_r( at )



# For simplicity to begin with connect all pairs of closest bodies.
func compute_joints():
	pass
