
extends ForceSource
class_name ForceSourceAirDrag


# Constants defining when normal force shielding is used.
const BACK_DIST: float    = 0.05
const FORWARD_DIST: float = 1.0


# Viscosity.and dynamic pressure.
var ground_level: float    = 100.0
var atm_height: float      = 10.0

var viscosity_gnd: float   = 0.1 * 0.001
var density_gnd: float     = 1.0 * 0.001

var normal_coef: float     = 0.5
var antinormal_coef: float = 0.9


func density_viscosity( se3: Se3Ref ):
	var r: Vector3  = se3.r
	var dist: float = r.length()
	var h: float = dist - ground_level
	if h <= 0.0:
		return [true, density_gnd, viscosity_gnd]
	elif h >= atm_height:
		return [false, 0.0, 0.0]
	
	var x: float = (h - atm_height) / atm_height
	var x2: float = x*x
	var dens: float = density_gnd*x2
	var visc: float = viscosity_gnd*x2
	
	return [true, dens, visc]




@export var ang_velocity: Vector3 = Vector3.ZERO

func compute_force( body: RefFrameNode, se3: Se3Ref ):
	var dv: Array = density_viscosity( se3 )
	var inside_atmosphere: bool = dv[0]
	if not inside_atmosphere:
		return [Vector3.ZERO, Vector3.ZERO]
	
	var mesh: OctreeMeshGd = body.get_octree_mesh()
	if mesh == null:
		return [Vector3.ZERO, Vector3.ZERO]
	
	# Air density and viscosity at current height.
	var dens: float = dv[1]
	var visc: float = dv[2]
	
	var qty: int = mesh.faces_qty()
	
	var F: Vector3 = Vector3.ZERO
	var P: Vector3 = Vector3.ZERO
	
	# Velocity of placet center with respect to 
	# The body.
	var vo: Vector3 = se3.v
	# Need to compute velocity of the point where the body 
	# is with respect to the body. In order to do that 
	# do the following: v = v - w.cross( r ). Subtract because 
	# body is ar "-r" relative to planet center.
	var wo: Vector3 = se3.w
	var ro: Vector3 = se3.r
	var v: Vector3 = vo - wo.cross( ro )
	
	var body_se3: Se3Ref = body.get_se3()
	# To convert velocity to parent ref. frame in order to query for 
	# face ray intersections.
	var body_q: Quaternion = body_se3.q
	var forward: Vector3 = -v.normalized()
	forward = body_q * (forward)
	
	var rf_physics: RefFrameNode = body.get_ref_frame_physics()
	var broad_tree: BroadTreeGd = rf_physics.get_broad_tree()
	
	for i in range(qty):
		var face: Array = mesh.get_face( i )
		var r: Vector3 = face[0]
		var n: Vector3 = face[1]
		var s: float   = face[2]
		
		var n_v: float = n.dot( v )
		
		var rf_r: Vector3  = body_q * (r)
		var start: Vector3 = rf_r - forward * BACK_DIST
		var end: Vector3   = rf_r + forward * FORWARD_DIST
		
		var ok: bool
		if broad_tree != null:
			ok = broad_tree.intersects_segment( start, end, mesh )
		else:
			ok = false
		
		# Compute normal force only if this face is not shielded by another 
		# body.
		var force_n: Vector3
		if ok:
			force_n = Vector3.ZERO
		else:
			var coef: float
			if n_v > 0.0:
				coef = antinormal_coef
			else:
				coef = normal_coef
			
			var abs_force_n: float = -coef*dens*s*n_v*n_v
			force_n = abs_force_n * n
		
		var abs_force_l: float = visc*s
		var Vn: Vector3 = n_v * n
		var force_l: Vector3 = abs_force_l*(v - Vn)

		var force: Vector3 = force_n + force_l
		var torque: Vector3 = r.cross( force )
		
		F += force
		P += torque
	
	var ret: Array = []
	ret.push_back( F )
	ret.push_back( P )
	return ret











