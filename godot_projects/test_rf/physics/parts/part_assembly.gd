
extends Body
class_name PartAssembly

# This class handles dynamic body group.
# On every update it should average it's position to the mean of 
# all sub-bodies.




# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.



# Trying to override Body::_physics_process(). Will see if it works now in 3.2.3.
func physics_process_inner( delta ):
	pass



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






func create_physical():
	print( "PartSuperBody::create_physical()" )
	
	return null


func remove_physical():
	print( "PartSuperBody::remove_physical()" )
	#.remove_physical()


