
extends RefFrame
class_name CelestialBody

# Planet rotation.
export(float)   var rotation_period_hrs = 0.1
export(Vector3) var rotation_axis       = Vector3( 0.0, 1.0, 0.0 )

# Defining geometry and GM based on surface orbiting velocity.
export(float)  var surface_orbital_vel_kms = 0.3
export(float)  var planet_radius_km    = 1.0
export(float)  var planet_height_km    = 0.1
export(String) var height_source_name = "test"

# Planet's orbit parameters.
export(Vector3) var perigee_dir = Vector3( 1.0, 0.0, 0.0 )
export(Vector3) var perigee_vel = Vector3( 0.0, 0.0, -1.0 )
export(float)   var orbital_period_hrs   = 0.1
export(float)   var orbital_eccentricity = 0.0

# Subdivision levels.
export(float) var detail_size_0 = 5.0
export(float) var detail_dist_0 = 100.0
export(float) var detail_size_1 = 20.0
export(float) var detail_dist_1 = 300.0
export(float) var detail_size_2 = 100.0
export(float) var detail_dist_2 = 1e10

var initialized: bool = false
var gm: float = -1.0
var motion: CelestialMotionRef = null

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass



func init():
	if initialized:
		return
	initialized = true
	
	motion = CelestialMotionRef.new()
	
	# Initialize GM.
	gm = motion.init_gm( planet_radius_km, surface_orbital_vel_kms )
	
	# Initialize orbital movement.
	perigee_dir = perigee_dir.normalized()
	perigee_vel = perigee_dir - perigee_vel.project( perigee_dir )
	perigee_vel = perigee_vel.normalized()
	
	var p = get_parent()
	var parent_body: CelestialBody = p as CelestialBody
	if parent_body != null:
		parent_body.init()
		var parent_gm: float = parent_body.gm
		motion.launch_elliptic( parent_gm, perigee_dir, perigee_vel, orbital_period_hrs, orbital_eccentricity )
	
	# Initialize rotation.

