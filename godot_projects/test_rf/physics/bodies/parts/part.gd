
extends Body
class_name Part

enum PartClass {
	THRUSTER=0, 
	FUEL_TANK=1, 
	SRB_TANK=2, 
	REACTION_CONTROL_WHEELS=3, 
	REACTION_CONTROL_THRUSTER=4, 
	HABITAT=5, 
	PASSIVE=6 # This one just means no predefined functionality.
 }

export(PartClass) var part_class = PartClass.THRUSTER
export(bool) var allows_surface_attachments=true
export(bool) var allows_y_radial_symmetry=true
export(bool) var allows_x_mirror_symmetry=true
export(bool) var conducts_liquid_fuel=true
export(bool) var conducts_solid_fuel=true
export(bool) var conducts_electricity=true
export(bool) var conducts_air=true

var stacking_nodes: Array = []
var surface_nodes: Array  = []



# If contains Spatial with prefix "stacking_node" it's position and Y axis direction 
# define the stacking node. Stackig nodes can be coupled with each other.
# If it contains Spatial with prefix "surface_node" defines a surface coupling node. 
# A part can use this node to get attached to another part's surface which allows such 
# conenctions.
func _traverse_coupling_nodes():
	_traverse_coupling_nodes_recursive( self )



func _traverse_coupling_nodes_recursive( p: Node ):
	var s: Spatial = p as Spatial
	if s != null:
		var stacking_node: PartNodeStacking = s as PartNodeStacking
		var is_stacking_node: bool = (stacking_node != null)
		var is_surface_node: bool = false
		if not is_stacking_node:
			var surface_node: PartNodeSurface = s as PartNodeSurface
			is_surface_node = (surface_node != null)
		if is_stacking_node or is_surface_node:
			if is_stacking_node:
				stacking_nodes.push_back( s )
			else:
				surface_nodes.push_back( s )
	
	var qty: int = get_child_count()
	for i in range( qty ):
		var ch: Node = get_child( i )
		_traverse_coupling_nodes_recursive( ch )



# When being constructed podies are not supposed to move.
# So it is possible to make dynamic bodies kinematic.
# And when editing is done, one can switch those back to 
# being dynamic.
# These two should be overwritten.
func activate():
	if _physical != null:
		_physical.mode = RigidBody.MODE_RIGID
		_physical.sleeping = false
	
	for body in sub_bodies:
		body.activate()



func deactivate():
	if _physical != null:
		_physical.mode = RigidBody.MODE_KINEMATIC
	
	for body in sub_bodies:
		body.deactivate()





func init():
	.init()
	_traverse_coupling_nodes()



# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass




