
extends RefFrameMotionNode
class_name CelestialBody

@export var wiki_page: String = ""
@export var discovered: bool = false

# Planet rotation.
@export var rotation_period_hrs: float = 0.1
@export var rotation_axis: Vector3       = Vector3( 0.0, 1.0, 0.0 )

# Defining geometry and GM based on surface orbiting velocity.
@export var surface_source: Resource = null
#export(float)  var surface_orbital_vel_kms = 0.3

# Planet's orbit parameters.
@export var perigee_dir: Vector3 = Vector3( 1.0, 0.0, 0.0 )
@export var perigee_vel: Vector3 = Vector3( 0.0, 0.0, -1.0 )
@export var orbital_period_hrs: float   = 0.1
@export var orbital_eccentricity: float = 0.0

# If solar system is not constructed very carefully, 
# sometimes the body of the biggest influence can be current body's 
# parent even if currently moving within local moon system (within moon orbit).
# This situation would not be possible in reality, because the moon would 
# escape he planet in this situation. But, due to idealized Kepler orbits are 
# used, here it is possible.
# To tolerate this problem, artificially extend thpere of influence of 
# current body to the most distant moon semimajor axis x1.5 when considering 
# switching to current body's parent.
var _min_sphere_of_influence_to_parent: float = -1.0
var _all_parents: Array = []


var initialized: bool = false




#func get_class():
#	return "CelestialBody"

# Called when the node enters the scene tree for the first time.
func _ready():
	add_to_group( Constants.CELESTIAL_BODIES_NAME )
	# This must be called in the derived classes.
	#init()



func init():
	pass



func init_forces():
#	if initialized:
#		return
#	initialized = true
	
	# Initialize GM.
	#init_force_source()
	pass



# This thing is supposed to process physics ref frames which are children of the 
# celestial body. Need to compute influence and change parent or allow/disallow orbiting.
func process_ref_frames( celestial_bodies: Array ):
	# Apply gravity to physics bodies here.
	var bodies: Array = get_all_physics_bodies( self, true, false )
	set_local_up( bodies, self )


# Returns all physics ref frames for a node.
func get_ref_frames( n: Node, certain_ones: bool = false, orbiting: bool = false ):
	#var children: Array = n.get_children()
	var qty: int = n.get_child_count()
	var rfs: Array = []
	#for ch in children:
	for i in qty:
		var ch: Node = n.get_child( i )
		var rfp: RefFramePhysics = ch as RefFramePhysics
		if rfp == null:
			continue
		
		if not certain_ones:
			rfs.push_back( rfp )
		
		else:
			var rf_orbiting: bool = rfp.is_orbiting()
			var matches: bool = (rf_orbiting == orbiting)
			if matches:
				rfs.push_back( rfp )
			
	return rfs


func get_all_physics_bodies( n: Node, certain_ones: bool = false, orbiting: bool = false ):
	var ref_frames: Array = get_ref_frames( n, certain_ones, orbiting )
	var physics_bodies: Array = []
	for rf in ref_frames:
		var bodies: Array = rf.child_physics_bodies()
		physics_bodies += bodies
	
	return physics_bodies



func gravitational_influence( se3: Se3Ref ):
	var r: Vector3 = se3.r
	var dist2 = r.length_squared()
	var gm: float = get_own_gm()
	var ret: float = gm/dist2
	return ret






func _serialize( data: Dictionary ):
	pass


func _deserialize( data: Dictionary ):
	return true




func set_local_up( physics_bodies: Array, force_origin: RefFrameNode ):
	# For each body apply the force source.
	for b in physics_bodies:
		var se3_rel_to_origin: Se3Ref = self.relative_to( b )
		var se3_local: Se3Ref = b.get_se3()
		var q: Quaternion = se3_local.q
		
		var r: Vector3 = -se3_rel_to_origin.r
		r = r.normalized()
		r = q * (r)
		b.set_local_up( r )



func compute_min_shpere_of_influence_to_parent( force_recompute: bool = false ):
	#_min_sphere_of_influence_to_parent
	if (_min_sphere_of_influence_to_parent > 0.0) and (not force_recompute):
		return [_min_sphere_of_influence_to_parent, _all_parents]
	
	var parent: CelestialBody = get_parent() as CelestialBody
	if parent == null:
		_min_sphere_of_influence_to_parent = 0.0
		_all_parents = []
		return
	
	# Fill in all parents.
	while parent != null:
		_all_parents.push_back( parent )
		parent = parent.get_parent() as CelestialBody
	
	# Get all children. And compute the furthest apogee.
	var child_bodies: Array = []
	var child_qty: int = get_child_count()
	var max_dist: float = 0.0
	for i in range(child_qty):
		var child_body: CelestialBody = get_child(i) as CelestialBody
		if child_body == null:
			continue
		# For now only celestial surface has motion.
		if "motion" in child_body:
			var m: CelestialMotionRef = child_body.motion
			var a: float = m.apogee()
			if max_dist < a:
				max_dist = a
	
	# Slightly larget. 1.3 looks like a decent number.
	_min_sphere_of_influence_to_parent = max_dist * 1.3
	
	return [_min_sphere_of_influence_to_parent, _all_parents]








