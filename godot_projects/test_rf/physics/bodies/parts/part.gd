extends Body

enum PartClass {
	THRUSTER=0, 
	FUEL_TANK=1, 
	SRB_TANK=2, 
	REACTION_CONTROL_WHEELS=3, 
	REACTION_CONTROL_THRUSTER=4, 
	HABITAT=5
 }

export(PartClass) var part_class = PartClass.THRUSTER
export(bool) var allows_surface_attachments=true
export(bool) var allows_y_radial_symmetry=true
export(bool) var allows_x_mirror_symmetry=true
export(bool) var conducts_liquid_fuel=true
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

func _traverse_coupling_nodes_recursive( p: Node, t_to_root: Transform=Transform.IDENTITY ):
	var s: Spatial = p as Spatial
	if s != null:
		var name: String = s.name
		var is_stacking_node: bool = name.begins_with( "stacking_node" )
		var is_surface_node: bool = false
		if not is_stacking_node:
			is_surface_node = name.begins_with( "surface_node" )
		var t: Transform = s.transform
		t_to_root = t_to_root * t
		if is_stacking_node or is_surface_node:
			var r: Vector3 = t_to_root.origin
			var q: Quat = t_to_root.basis.get_rotation_quat()
			var e: Vector3 = q.xform( Vector3.UP )
			
			var n: Dictionary = {
				r=r, 
				e=e
			}
			if is_stacking_node:
				stacking_nodes.push_back( n )
			else:
				surface_nodes.push_back( n )
	
	var qty: int = get_child_count()
	for i in range( qty ):
		var ch: Node = get_child( i )
		_traverse_coupling_nodes_recursive( ch, t_to_root )



func init():
	.init()
	_traverse_coupling_nodes()

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
