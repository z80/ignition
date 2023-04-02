
extends    CelestialBody
class_name Sun


# Defining geometry and GM based on surface orbiting velocity.
@export var radius_km: float = 5.0

@export var glow_size: float = 0.2
@export var ray_scale: float = 10.0
@export var ray_size: float  = 1.0
@export var ray_bias: float  = 0.1

var ref_frame_to_check_index: int = 0


#func get_class():
#	return "Sun"


# Called when the node enters the scene tree for the first time.
func _ready():
	init_forces()


func init_forces():
	add_to_group( Constants.SUN_GROUP_NAME )
	
	# Initialize GM.
	var motion: CelestialMotionRef = CelestialMotionRef.new()
	
	var radius_km: float               = surface_source.radius_km
	var surface_orbital_vel_kms: float = surface_source.orbiting_velocity * 0.001
	
	var gm: float = motion.compute_gm_by_speed( radius_km, surface_orbital_vel_kms )
	set_own_gm( gm )
	
	super.init_forces()




func process_ref_frames( celestial_bodies: Array ):
	super.process_ref_frames( celestial_bodies )
	
	var rfs: Array = get_ref_frames( self )
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
	var data: Dictionary = super.serialize()
	
	return data




func deserialize( data: Dictionary ):
	var ret: bool = super.deserialize( data )
	if not ret:
		return false
	init_forces()
	return true









