

static func cluster( rf: RefFramePhysics, src: Array, dest: Array, ret: Array ) -> float:
	return -1.0


static func cluster_score( rf: RefFramePhysics, objs: Array, split_ind: int ) -> float:
	var score: float = 0.0
	var upper_bound: int = split_ind - 1
	for i in range(upper_bound):
		var body_a: Body = objs[i]
		var lower_bound: int = i+1
		for j in range(lower_bound, upper_bound):
			var body_b: Body = objs[j]
			var d: float = body_a.distance_max( body_b )
			score += d
	
	return score
	
	
	
static func cluster_dist( rf: RefFramePhysics, objs: Array, split_ind: int ) -> float:
	var min_dist: float = -1.0
	var qty: int = objs.size()
	for i in range(split_ind):
		var body_a: Body = objs[i]
		for j in range(split_ind, qty):
			var body_b: Body = objs[j]
			var d: float = body_a.distance(body_b)
			if (min_dist < 0.0) or (min_dist > d):
				min_dist = d
	return min_dist
	

static func cluster_swap( rf: RefFramePhysics, src: Array, dest: Array, element_ind: int, split_ind: int ) -> int:
	# Not sure if this will work.
	# If not, per element copy should.
	dest = src.duplicate()
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
	
	return split_ind



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

