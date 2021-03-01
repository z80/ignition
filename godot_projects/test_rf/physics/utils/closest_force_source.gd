
static func closest_force_source( me: Node ):
	var n: Node =  _force_source_recursive( me )
	if n == null:
		return null
	var fs: RefFrame = n as RefFrame
	return fs


static func _force_source_recursive( n: Node ):
	if n == null:
		return null
	
	var rf: RefFrame = n as RefFrame
	if rf != null:
		if rf.force_source != null:
			return rf
	
	var p: Node = n.get_parent()
	return _force_source_recursive( p )

