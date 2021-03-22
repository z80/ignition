

static func cluster( src_full: Array ) -> Array:
	var src: Array = filter_top_hierarchy( src_full )
	var dest: Array = []
	var qty: int = src.size()
	var split_ind: int = qty
	var dest_split_ind: int = qty
	var modified: bool = true
	var base_score: float = cluster_score( src, split_ind )
	var base_split_ind: int = split_ind
	while ( modified ):
		modified = false
		
		for i in range(qty):
			var ret: Array = cluster_swap( src, i, split_ind )
			dest_split_ind = ret[0]
			dest           = ret[1]
			var score: float = cluster_score( dest, dest_split_ind )
			if ( score < base_score ):
				split_ind = dest_split_ind
				src       = dest.duplicate()
				
				base_score = score
				base_split_ind = dest_split_ind
				modified = true
				break
	
	var dist = cluster_dist( src, base_split_ind )
	dest = src
	
	return [dist, base_split_ind, dest]


static func cluster_score( objs: Array, split_ind: int ) -> float:
	var score: float = 0.0
	var upper_bound: int = split_ind - 1
	for i in range(upper_bound):
		var body_a: Body = objs[i]
		var lower_bound: int = i+1
		for j in range(lower_bound, split_ind):
			var body_b: Body = objs[j]
			#var d: float = body_a.distance_max( body_b )
			var d: float = body_a.distance( body_b )
			score += d
	
	var qty: int = objs.size()
	upper_bound = qty - 1
	for i in range(split_ind, upper_bound):
		var body_a: Body = objs[i]
		var lower_bound = i+1
		for j in range(lower_bound, qty):
			var body_b: Body = objs[j]
			#var d: float = body_a.distance_max( body_b )
			var d: float = body_a.distance( body_b )
			score += d
	
	return score
	
	
	
static func cluster_dist( objs: Array, split_ind: int ) -> float:
	var min_dist: float = -1.0
	var qty: int = objs.size()
	for i in range(split_ind):
		var body_a: Body = objs[i]
		for j in range(split_ind, qty):
			var body_b: Body = objs[j]
			#var d: float = body_a.distance_min(body_b)
			var d: float = body_a.distance(body_b)
			if (min_dist < 0.0) or (min_dist > d):
				min_dist = d
	return min_dist
	

static func cluster_swap( src: Array, element_ind: int, split_ind: int ) -> Array:
	# Not sure if this will work.
	# If not, per element copy should.
	var dest: Array = src.duplicate()
	if element_ind < split_ind:
		var last_ind: int = split_ind - 1
		if element_ind < last_ind:
			var a: Body = dest[element_ind]
			var b: Body = dest[last_ind]
			dest[last_ind] = a
			dest[element_ind] = b
		split_ind -= 1
	else:
		var first_ind: int = split_ind
		if first_ind < element_ind:
			var a: Body = dest[first_ind]
			var b: Body = dest[element_ind]
			dest[first_ind] = b
			dest[element_ind] = a
		split_ind += 1
	
	return [split_ind, dest]



static func filter_top_hierarchy( src: Array ) -> Array:
	var dest: Array = []
	var qty: int = src.size()
	for i in range( qty ):
		var body: Body = src[i]
		var b = body.root_most_body()
		var ind: int = dest.find( b )
		if ( ind < 0 ):
			dest.push_back( b )
	
	return dest





