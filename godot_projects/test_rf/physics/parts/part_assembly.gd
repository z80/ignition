extends Body
class_name PartAssembly

# This class handles dynamic body group.
# On every update it should average it's position to the mean of 
# all sub-bodies.

# Also should keep connection information in order to create/destroy joints.
var MinSpanTree = preload( "res://physics/parts/min_span_tree.gd" )

# Indices which bodies to connect
var _edges: Array



# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func create_edges():
	_edges = MinSpanTree.mst_compute( sub_bodies )


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
	
	if _physical != null:
		return _physical
	
	_physical = PartAssemblyPhysical.new()
	var root = BodyCreator.root_node
	root.add_child( _physical )
	_physical.setup( sub_bodies, _edges )
	
	return _physical


func remove_physical():
	print( "PartSuperBody::remove_physical()" )
	.remove_physical()


