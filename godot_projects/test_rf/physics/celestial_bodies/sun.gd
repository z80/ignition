
extends    CelestialBody
class_name Sun


# Defining geometry and GM based on surface orbiting velocity.

export(float) var glow_size = 0.2
export(float) var ray_scale = 10.0
export(float) var ray_size  = 1.0
export(float) var ray_bias  = 0.1

var ref_frame_to_check_index: int = 0


func get_class():
	return "Sun"


# Called when the node enters the scene tree for the first time.
func _ready():
	init()


func init():
	add_to_group( Constants.SUN_GROUP_NAME )
	
	# Initialize GM.
	var motion: CelestialMotionRef = CelestialMotionRef.new()
	gm = motion.init_gm( radius_km, surface_orbital_vel_kms )
	
	.init()




func process_ref_frames( celestial_bodies: Array ):
	.process_ref_frames( celestial_bodies )
	var rfs: Array = ref_frames( self )
	var qty: int = len( rfs )
	if qty < 1:
		return
	if ref_frame_to_check_index >= qty:
		ref_frame_to_check_index = 0
	
	var rf: RefFramePhysics = rfs[ref_frame_to_check_index]
	# Determine distance to all bodies.
	var celestial_bodies_qty: int = celestial_bodies.size()
	var biggest_influence: float = -1.0
	var biggest_influence_body: CelestialBody = null
	for i in range( celestial_bodies_qty ):
		var cb: CelestialBody = celestial_bodies[i]
		var se3: Se3Ref = rf.relative_to( cb )
		var infl: float = cb.gravitational_influence( se3 )
		if (biggest_influence_body == null) or (biggest_influence < infl):
			biggest_influence = infl
			biggest_influence_body = cb
	
	# Check if the strongest influence is caused by other celestial body.
	if (biggest_influence_body != null) and (biggest_influence_body != self):
		# Need to teleport celestial body to that other celestial body
		rf.change_parent( biggest_influence_body )






func serialize():
	var data: Ditionary = .serialize()
	data["glow_size"] = glow_size
	data["ray_scale"] = ray_scale
	data["ray_size"]  = ray_size
	data["ray_bias"]  = ray_bias
	
	return data




func deserialize( data: Dictionary ):
	var ret: bool = .deserialize( data )
	if not ret:
		return false
	glow_size = data["glow_size"]
	ray_scale = data["ray_scale"]
	ray_size  = data["ray_size"]
	ray_bias  = data["ray_bias"]
	init()
	return true









