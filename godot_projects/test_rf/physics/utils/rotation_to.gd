
const EPS: float = 0.00001


static func rotation_to( v0: Vector3, v1: Vector3 ):
	var r: Vector3 = v0.cross( v1 )
	var sin_a: float = r.length()
	if sin_a < EPS:
		return Quat.IDENTITY
	
	var a: float = asin( sin_a )
	var a2: float = a * 0.5
	var si2: float = sin( a2 )
	var co2: float = cos( a2 )
	var k: float = si2 / sin_a
	var q: Quat = Quat( r.x*k, r.y*k, r.z*k, co2 )
	
	return q



# Local ref frame on a sphere.
# Y - local up.
# Z - away from the north polse.
# X - along local parallel.
static func local_ref_frame( n: Vector3 ):
	var n0: Vector3 = Vector3.UP
	var x: Vector3 = n.cross( n0 )
	var abs_x: float = x.length()
	var y: Vector3
	var z: Vector3
	if abs_x < EPS:
		y = Vector3.UP
		z = Vector3.BACK
		x = Vector3.RIGHT
	else:
		x = x / abs_x
		y = n
		z = x.cross( y )
	
	var b: Basis = Basis( x, y, z )
	var q: Quat = b.get_rotation_quat()
	return q





