
extends    RefFrame
class_name Sun


# Defining geometry and GM based on surface orbiting velocity.
export(float)  var surface_orbital_vel_kms = 0.3
export(float)  var planet_radius_km        = 1.0

export(float) var glow_size = 0.2
export(float) var ray_scale = 10.0
export(float) var ray_size  = 1.0
export(float) var ray_bias  = 0.1


var initialized: bool = false
var gm: float = -1.0


# Called when the node enters the scene tree for the first time.
func _ready():
	init()


func init():
	if initialized:
		return
	initialized = true
	
	add_to_group( Constants.SUN_GROUP_NAME )
	
	# Initialize GM.
	var motion: CelestialMotionRef = CelestialMotionRef.new()
	gm = motion.init_gm( planet_radius_km, surface_orbital_vel_kms )
	init_force_source()


func init_force_source():
	.init_force_source()
	force_source = ForceSourceGravity.new()
	force_source.GM = gm







