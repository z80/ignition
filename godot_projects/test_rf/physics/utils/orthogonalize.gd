
static func pick_basis( up: Vector3 ):
	var e: Array = [ Vector3(1.0, 0.0, 0.0), 
					 Vector3(0.0, 1.0, 0.0), 
					 Vector3(0.0, 0.0, 1.0) ]
	var max_dot: float = -1.0
	var max_dot_index: int = -1
	for i in range( 3 ):
		var d: float = up.dot( e[i] )
		if (max_dot_index < 0) or (max_dot < d):
			max_dot = d
			max_dot_index = i
	var e0: Vector3 = e[(max_dot_index+2) % 3]
	var e1: Vector3 = up.normalized()
	var e2: Vector3 = e[(max_dot_index+1) % 3]
	
	var u1: Vector3 = e1
	
	var u0: Vector3 = e0 - e0.project(u1)
	u0 = u0.normalized()
	
	var u2: Vector3 = e2 - e2.project(u0) - e2.project(u1)
	u2 = u2.normalized()
	
	return [u0, u1, u2]


static func adjust_basis( e0: Vector3, e2: Vector3, up: Vector3 ):
	var u1: Vector3 = up
	
	var u0: Vector3 = e0 - e0.project(u1)
	u0 = u0.normalized()
	
	var u2: Vector3 = e2 - e2.project(u0) - e2.project(u1)
	u2 = u2.normalized()
	
	return [u0, u1, u2]
	




static func orthogonalize( e1: Vector3, e2: Vector3, e3: Vector3 ):
	var u1: Vector3 = e1
	u1 = u1.normalized()
	
	var u2: Vector3 = e2 - e2.project(u1)
	u2 = u2.normalized()
	
	var u3: Vector3 = e3 - e3.project(u1) - e3.project(u2)
	u3 = u3.normalized()
	
	var res = [ u1, u2, u3 ]
	return res
















