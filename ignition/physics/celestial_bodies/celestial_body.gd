
extends RefFrame
class_name CelestialBody

# Planet rotation.
export(float)   var rotation_period_hrs = 0.1
export(Vector3) var rotation_axis       = Vector3( 0.0, 1.0, 0.0 )

# Defining geometry and GM based on surface orbiting velocity.
export(float)  var surface_orbital_vel_kms = 0.3
export(float)  var radius_km    = 1.0
export(float)  var height_km    = 0.1

# Planet's orbit parameters.
export(Vector3) var perigee_dir = Vector3( 1.0, 0.0, 0.0 )
export(Vector3) var perigee_vel = Vector3( 0.0, 0.0, -1.0 )
export(float)   var orbital_period_hrs   = 0.1
export(float)   var orbital_eccentricity = 0.0


var initialized: bool = false
var gm: float = -1.0

# Gravity force source.
var force_source_gravity: ForceSourceGravity = null
## Centrifugal, Coriolis and Euler forces in rotating ref. frame.
#var force_source_inertial = null
## Atmosphere friction forces.
#var force_source_atmosphere_drag = null



func get_class():
	return "CelestialBody"

# Called when the node enters the scene tree for the first time.
func _ready():
	add_to_group( Constants.CELESTIAL_BODIES_NAME )
	init()


func init_force_source():
	.init_force_source()
	
	force_source_gravity = ForceSourceGravity.new()
	force_source_gravity.GM = gm






func init():
#	if initialized:
#		return
#	initialized = true
	
	# Initialize GM.
	init_force_source()



# This thing is supposed to process physics ref frames which are children of the 
# celestial body. Need to compute influence and change parent or allow/disallow orbiting.
func process_ref_frames( celestial_bodies: Array ):
	# Apply gravity to physics bodies here.
	var bodies: Array = get_all_physics_bodies( self, true, false )
	apply_forces( bodies, self, force_source_gravity, true )


# Returns all physics ref frames for a node.
func get_ref_frames( n: Node, certain_ones: bool = false, orbiting: bool = false ):
	var children = n.get_children()
	var rfs = []
	for ch in children:
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
	var ret: float = gm/dist2
	return ret






func serialize():
	var data: Dictionary = .serialize()
	return data



func deserialize( data: Dictionary ):
	var ret: bool = .deserialize( data )
	return ret




func apply_forces( physics_bodies: Array, force_origin: RefFrameNode, force_source: ForceSource, set_local_up: bool ):
	# For each body apply the force source.
	for b in physics_bodies:
		var se3_rel_to_origin: Se3Ref = self.relative_to( b )
		var se3_local: Se3Ref = b.get_se3()
		var q: Quat = se3_local.q
		var force_torque: Array = force_source.compute_force( b, se3_rel_to_origin )
		var F: Vector3 = force_torque[0]
		var P: Vector3 = force_torque[1]
		F = q.xform( F )
		P = q.xform( P )
		b.add_force_torque( F, P )
		
		if set_local_up:
			var r: Vector3 = -se3_rel_to_origin.r
			r = r.normalized()
			r = q.xform( r )
			b.set_local_up( r )





