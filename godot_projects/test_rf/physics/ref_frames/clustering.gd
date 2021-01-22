

static func cluster( rf: RefFramePhysics, src: Array, dest: Array, ret: Array ) -> float:
	return -1.0

static func cluster_score( rf: RefFramePhysics, objs: Array, split_ind: int ) -> float:
	return -1.0
	
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
	

static func cluster_swap( rf: RefFramePhysics, src: Array, dest: Array, element_ind: int ) -> int:
	return -1



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

