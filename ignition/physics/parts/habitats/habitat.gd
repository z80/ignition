
extends Part
class_name Habitat

# How many characters fit inside.
@export var capacity: int = 1

var entrance_nodes: Array = []
var characters_inside: Array = []


func init():
	super.init()
	# This is needed to get number of nodes through which a character
	# is suppsed to get in or out.
	_traverse_entrance_nodes()



# Called when the node enters the scene tree for the first time.
func _ready():
	super._ready()


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
	var s: Node3D = p as Node3D
	if s != null:
		var node: EntranceNode = s as EntranceNode
		var is_node: bool = (node != null)
		if is_node:
			# Add to the list.
			entrance_nodes.push_back( s )
			# And provide self as the part.
			s.part = self

	
	var qty: int = p.get_child_count()
	for i in range( qty ):
		var ch: Node = p.get_child( i )
		_traverse_entrance_nodes_recursive( ch )



func gui_classes( mode: Array ):
	var classes: Array = []
	# Adding habitat specific classes prior to generic classes.
	var C = load( "res://physics/parts/habitats/gui_elements/gui_habitat.tscn" )
	classes.push_back( C )
	# Adding generic classes.
	var classes_base: Array = super.gui_classes( mode )
	classes = classes + classes_base
	return classes



func characters_for_boarding():
	var current_qty: int = characters_inside.size()
	if current_qty >= capacity:
		return null
	
	var can_board: Array = []
	for n in entrance_nodes:
		var candidate: Node = n.characters_for_boarding()
		if candidate == null:
			continue
		
		var already_inside: int = characters_inside.has( candidate )
		if not already_inside:
			can_board.push_back( candidate )
	
	return can_board



func characters_for_unboarding():
	return characters_inside




func let_character_in( character ):
	character.set_boarding_mode_inside()
	characters_inside.push_back( character )
	# Also make habitat selected.
	RootScene.ref_frame_root.player_control = self
	RootScene.ref_frame_root.player_select  = self


func let_character_out( ind: int ):
	var character: Node = characters_inside[ind]
	characters_inside.remove_at( ind )
	
	var en: EntranceNode = entrance_nodes[0]
	var t: Transform3D = en.relative_to_owner
	var rel_se3: Se3Ref = Se3Ref.new()
	rel_se3.transform = t
	var own_se3: Se3Ref = self.get_se3()
	var total_se3 = own_se3.mul( rel_se3 )
	character.set_se3( total_se3 )
	
	var character_p: Node = character.get_parent()
	var needed_p: Node = self.get_parent()
	if needed_p != character_p:
		character.change_parent( needed_p )
	
	character.set_boarding_mode_outside()




func process_inner( delta: float ):
	super.process_inner( delta )
	_process_boarded_characters()




func _process_boarded_characters():
	var se3: Se3Ref = self.get_se3()
	for ch in characters_inside:
		ch.set_se3( se3 )





func _serialize( var data: Dictionary ):
	var paths: Array = []
	for ch in characters_inside:
		var path: String = ch.get_path()
		paths.push_back( path )
	data["characters_inside"] = paths
	
	return data



func _deserialize( data: Dictionary ):
	var ok: bool = super.deserialize( data )
	
	if data.has( "characters_inside" ):
		characters_inside.clear()
		var paths: Array = data["characters_inside"]
		for p in paths:
			var n: Node = get_node( p )
			characters_inside.push_back( n )
	
	return ok


