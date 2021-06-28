
static func closest_celestial_body( me: Node ):
	var n: Node =  _celestial_body_recursive( me )
	if n == null:
		return null
	return n


static func _celestial_body_recursive( n: Node ):
	if n == null:
		return null
	
	var cb: RefFrameNode = n as CelestialSurface
	
	if (cb != null) and (cb.get_class() == "CelestialSurface"):
		return cb
	
	# Try the owner (root of local scene 
	# created during scene design).
	var o: Node = n.owner
	if o != null:
		cb = o as CelestialSurface
		if cb != null:
			return cb
	
	var p: Node = n.get_parent()
	return _celestial_body_recursive( p )
