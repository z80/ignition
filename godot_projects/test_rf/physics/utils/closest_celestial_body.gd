
static func closest_celestial_body( me: Node ):
	var n: Node =  _celestial_body_recursive( me )
	if n == null:
		return null
	return n


static func _celestial_body_recursive( n: Node ):
	if n == null:
		return null
	
	var cb: CelestialSurface = n as CelestialSurface
	if cb != null:
		return n
	
	var p: Node = n.get_parent()
	return _celestial_body_recursive( p )
