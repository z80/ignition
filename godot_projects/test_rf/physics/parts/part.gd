
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


enum PartMode {
	CONSTRUCTION=0, 
	SIMULATION=1
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

var mode: int = PartMode.SIMULATION


# If contains Spatial with prefix "stacking_node" it's position and Y axis direction 
# define the stacking node. Stackig nodes can be coupled with each other.
# If it contains Spatial with prefix "surface_node" defines a surface coupling node. 
# A part can use this node to get attached to another part's surface which allows such 
# conenctions.
func _traverse_coupling_nodes():
	_traverse_coupling_nodes_recursive( _visual )



func _traverse_coupling_nodes_recursive( p: Node ):
	var s: Spatial = p as Spatial
	if s != null:
		var stacking_node: CouplingNodeStacking = s as CouplingNodeStacking
		var is_stacking_node: bool = (stacking_node != null)
		var is_surface_node: bool = false
		if not is_stacking_node:
			var surface_node: CouplingNodeSurface = s as CouplingNodeSurface
			is_surface_node = (surface_node != null)
		if is_stacking_node or is_surface_node:
			# Specify the reference to itself.
			# Not sure if it is 100% needed but just in case.
			s.part = self
			if is_stacking_node:
				stacking_nodes.push_back( s )
			else:
				surface_nodes.push_back( s )
	
	var qty: int = p.get_child_count()
	for i in range( qty ):
		var ch: Node = p.get_child( i )
		_traverse_coupling_nodes_recursive( ch )


func process_inner( _delta ):
	.process_inner( _delta )
	var qty: int = stacking_nodes.size()
	for i in range(qty):
		var n: CouplingNodeStacking = stacking_nodes[i]
		n.process()



# When being constructed parts are not supposed to move.
# So it is possible to make dynamic bodies kinematic.
# And when editing is done, one can switch those back to 
# being dynamic.
# These two should be overwritten.
func activate( root_call: bool = true ):
	var is_activated: bool = (body_state == BodyState.DYNAMIC)
	if is_activated:
		return
	
	.activate( root_call )
	
	# If activated, not in construction anymore.
	mode = PartMode.SIMULATION
	
	# Get all connected parts and switch them as well.
	var nodes_qty: int = stacking_nodes.size()
	for i in range(nodes_qty):
		var n: CouplingNodeStacking = stacking_nodes[i]
		var c: bool = n.connected()
		if not c:
			continue
		var p: Part = n.part_b.part
		p.activate( false )
	
	for i in range(nodes_qty):
		var n: CouplingNodeStacking = stacking_nodes[i]
		var c: bool = n.connected()
		if not c:
			continue
		if not n.is_parent:
			continue
		n.activate()
	
	# Create super body. Through it controls are computed and 
	# merge/split ref frames are computed.
	if root_call:
		var sb = PartAssembly.new()
		BodyCreator.root_node.add_child( sb )
		var p = self.get_parent()
		sb.debug = true
		sb.change_parent( p )
		sb.debug = false
		var se3: Se3Ref = self.get_se3()
		sb.set_se3( se3 )
		
		# Add all bodies to this super body.
		var ret: Array = dfs_search( self )
		var bodies: Array = ret[1]
		for body in bodies:
			sb.add_sub_body( body )
		
		# Now sure what this one is doing.
		#sb.activate()




func deactivate( root_call: bool = true ):
	.deactivate(root_call)
	
		# Get all connected parts and switch them as well.
	var nodes_qty: int = stacking_nodes.size()
	for i in range(nodes_qty):
		var n: CouplingNodeStacking = stacking_nodes[i]
		var c: bool = n.connected()
		if not c:
			continue
		if not n.is_parent:
			continue
		n.deactivate()
	
	for i in range(nodes_qty):
		var n: CouplingNodeStacking = stacking_nodes[i]
		var c: bool = n.connected()
		if not c:
			continue
		var p: Part = n.part_b.part
		p.deactivate( false )
	
	# Need to make sure that not in a construction mode.
	# In construction mode super boy is provided externally 
	# And provides additional context menu GUIs.
	if root_call and (mode != PartMode.CONSTRUCTION):
		if super_body != null:
			super_body.queue_free()
			super_body = null




func set_show_node_visuals( en: bool ):
	for n in stacking_nodes:
		n.show_visual = en
	for n in surface_nodes:
		n.show_visual = en


func couple():
	# Get parent ref frame, get all parts in it.
	var rf: RefFrameNode = get_parent()
	var parts: Array = []
	var qty: int = rf.get_child_count()
	for i in range(qty):
		var ch: Node = rf.get_child( i )
		var p: Part = ch as Part
		if (p != null) and (p != self):
			parts.push_back( p )
	
	var parts_qty: int = parts.size()
	
	var done: bool = false
	var own_nodes_qty: int = stacking_nodes.size()
	for own_node_ind in range(own_nodes_qty):
		var own_node: CouplingNodeStacking = stacking_nodes[own_node_ind]
		# Skip the already coupled ones.
		if own_node.part_b != null:
			continue
		
		for part_ind in range(parts_qty):
			var part: Part = parts[part_ind]
			var other_nodes_qty: int = part.stacking_nodes.size()
			for other_node_ind in range(other_nodes_qty):
				var other_node: CouplingNodeStacking = part.stacking_nodes[other_node_ind]
				# If already coupled, skip it.
				if other_node.part_b != null:
					continue
				
				# Try couple.
				var ret: bool = own_node.couple_with( other_node )
				if ret:
					done = true
					break
			
			if done:
				break
	
	return done



func decouple():
	var own_nodes_qty: int = stacking_nodes.size()
	for own_node_ind in range( own_nodes_qty ):
		var own_node: CouplingNodeStacking = stacking_nodes[own_node_ind]
		# Skip the already coupled ones.
		if (own_node.part_b != null) or (own_node.is_parent):
			continue
		own_node.decouple()
		return true
	return false




func init():
	.init()
	_traverse_coupling_nodes()



# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass






# Depth first search for all the parts connected with each other.
static func dfs_search( p: Part ):
	var root_part: Part = _find_root( p )
	var parts: Array = []
	_dfs( root_part, parts )
	
	var ret: Array = [ root_part, parts ]
	return ret




# Called when the node enters the scene tree for the first time.
# It's not formally needed to start with root.
# It is only done for extra convenience to know what part is the root.
static func _find_root( p: Part ):
	var part: Part = p
	var again: bool = true
	while again:
		again = false
		# Check all the connected nodes.
		var qty: int = part.surface_nodes.size()
		for i in range(qty):
			var n: CouplingNodeSurface = part.surface_nodes[i]
			var connected: bool = n.connected()
			if not connected:
				continue
			var is_parent: bool = n.is_parent
			if is_parent:
				continue
			# Switch to this node's part.
			part = n.part
			again = true
			break
	return p



static func _dfs( part: Part, parts: Array ):
	var already_there: bool = parts.has( part )
	if already_there:
		return
	
	parts.push_back( part )
	
	var qty: int = part.stacking_nodes.size()
	for i in range(qty):
		var n: CouplingNodeStacking = part.stacking_nodes[i]
		var connected: bool = n.connected()
		if not connected:
			continue
		var is_parent: bool = n.is_parent
		if not is_parent:
			continue
		
		var p: Part = n.part
		_dfs( p, parts )



