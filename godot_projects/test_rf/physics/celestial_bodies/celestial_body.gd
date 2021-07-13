
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


func get_class():
	return "CelestialBody"

# Called when the node enters the scene tree for the first time.
func _ready():
	add_to_group( Constants.CELESTIAL_BODIES_NAME )
	init()


func init_force_source():
	.init_force_source()
	
	force_source = ForceSourceGravity.new()
	force_source.GM = gm






func init():
#	if initialized:
#		return
#	initialized = true
	
	# Initialize GM.
	init_force_source()



# This thing is supposed to process physics ref frames which are children of the 
# celestial body. Need to compute influence and change parent or allow/disallow orbiting.
func process_ref_frames( celestial_bodies: Array ):
	pass



# Returns all physics ref frames for a node.
func ref_frames( n: Node ):
	var children = n.get_children()
	var rfs = []
	for ch in children:
		var rfp: RefFramePhysics = ch as RefFramePhysics
		if rfp != null:
			rfs.push_back( rfp )
	return rfs


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





