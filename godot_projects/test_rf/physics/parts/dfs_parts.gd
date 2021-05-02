


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






