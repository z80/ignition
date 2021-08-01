
extends Part
class_name Habitat

# How many characters fit inside.
export(int) var capacity = 1

var entrance_nodes: Array = []
var characters_inside: Array = []


func init():
	.init()
	# This is needed to get number of nodes through which a character
	# is suppsed to get in or out.
	_traverse_entrance_nodes()



# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()


func can_enter( character: Part ):
	var qty: int = characters_inside.size()
	if qty >= capacity:
		return false
	
	var se3: Se3Ref = Se3Ref.new()
	
	for n in entrance_nodes:
		var node: EntranceNode = n
		se3.transform = node.relative_to_owner
		se3 = character.relative_to_se3( self, se3 )
		var dist: float = se3.r.length()
		var max_dist: float = node.compute_node_size()
		if dist <= max_dist:
			return true
	
	return false


# Called by gui creating thing.
func _traverse_entrance_nodes():
	_traverse_entrance_nodes_recursive( _visual )



func _traverse_entrance_nodes_recursive( p: Node ):
	var s: Spatial = p as Spatial
	if s != null:
		var node: EntranceNode = s as EntranceNode
		var is_node: bool = (node != null)
		if is_node:
			entrance_nodes.push_back( s )
	
	var qty: int = p.get_child_count()
	for i in range( qty ):
		var ch: Node = p.get_child( i )
		_traverse_entrance_nodes_recursive( ch )



func gui_classes( mode: Array ):
	var classes: Array = []
	var classes_base: Array = .gui_classes( mode )
	return classes



func _can_board():
	var current_qty: int = characters_inside.size()
	if current_qty >= capacity:
		return null
	
	var can_board: Array = []
	for n in entrance_nodes:
		var candidate: Node = n.boarding_available()
		if candidate == null:
			continue
		
		can_board.push_back( candidate )
	
	return can_board



func _can_unboard():
	return characters_inside

