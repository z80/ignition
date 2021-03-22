
extends    CelestialBody
class_name Sun


# Defining geometry and GM based on surface orbiting velocity.

export(float) var glow_size = 0.2
export(float) var ray_scale = 10.0
export(float) var ray_size  = 1.0
export(float) var ray_bias  = 0.1




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
	gm = motion.init_gm( radius_km, surface_orbital_vel_kms )
	
	.init()









