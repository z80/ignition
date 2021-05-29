

# Computing minimum spanning tree.
static func mst_compute( bodies: Array ):
	var in_tree: Array = []
	for b in bodies:
		in_tree.push_back( false )
	
	in_tree[0] = true
	var edges: Array = []
	
	var res: bool = true
	while res:
		res = add_closest( bodies, in_tree, edges )
	
	return edges



static func add_closest( bodies: Array, in_tree: Array, edges: Array ):
	var ind_i: int = 0
	var closest_dist: float = -1.0
	var closest_ind_i: int = -1
	var closest_ind_j: int = -1
	for in_tree_i in in_tree:
		if in_tree_i == true:
			var ind_j: int = 0
			for in_tree_j in in_tree:
				if in_tree_j == false:
					var body_i = bodies[ind_i]
					var body_j = bodies[ind_j]
					var dist = body_i.distance_min( body_j )
					if (closest_ind_j < 0) or (dist < closest_dist):
						closest_ind_i = ind_i
						closest_ind_j = ind_j
						closest_dist = dist
				ind_j += 1
		
		ind_i += 1
	
	if closest_ind_i >= 0:
		in_tree[closest_ind_j] = true
		var edge: Array = [closest_ind_i, closest_ind_j]
		edges.push_back( edge )
		return true
	
	return false






