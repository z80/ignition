
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
export(float) var detail_size_0 = 10.0
export(float) var detail_dist_0 = 100.0
export(float) var detail_size_1 = 50.0
export(float) var detail_dist_1 = 1000.0
export(float) var detail_size_2 = 300.0
export(float) var detail_dist_2 = 1e10

var initialized: bool = false
var gm: float = -1.0
var motion: CelestialMotionRef = null
var rotation: CelestialRotationRef = null



# Called when the node enters the scene tree for the first time.
func _ready():
	init()


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
	rotation = CelestialRotationRef.new()
	rotation.init( rotation_axis, rotation_period_hrs )
	
	# Initialize body geometry
	var celestial_body = get_node( "Rotation/CelestialBody" )
	var radius: float  = planet_radius_km * 1000.0
	var height: float  = planet_height_km * 1000.0
	celestial_body.radius = radius
	celestial_body.height = height
	celestial_body.height_source   = height_source( height_source_name, radius, height )
	celestial_body.distance_scaler = PhysicsManager.distance_scaler
	
	celestial_body.apply_scale         = true
	celestial_body.scale_mode_distance = 3.0
	
	celestial_body.clear_levels()
	celestial_body.add_level( detail_size_0, detail_dist_0 )
	celestial_body.add_level( detail_size_1, detail_dist_1 )
	celestial_body.add_level( detail_size_2, detail_dist_2 )
	
	connect( "mesh_updated", self, "on_mesh_updated" )



func process( delta ):
	process_motion( delta )
	process_geometry()



func process_motion( delta ):
	var translation_rf = get_node( "." )
	var rotation_rf    = get_node( "Rotation" )
	motion.process_rf( delta, translation_rf )
	rotation.process_rf( delta, rotation_rf )



func process_geometry():
	var player_rf: RefFramePhysics = PhysicsManager.player_ref_frame
	var physics_ref_frames: Array  = PhysicsManager.physics_ref_frames()
	var planet: CubeSphereNode = get_node( "Rotation/CelestialBody" )
	
	planet.clear_ref_frames()
	planet.origin_ref_frame = player_rf.get_path()
	for rf in physics_ref_frames:
		var path: NodePath = rf.get_path()
		planet.add_ref_frame( path )






func set_origin( rf ):
	var celestial_body = get_node( "Rotation/CelestialBody" )
	var rf_path = rf.get_path()
	celestial_body.origin_ref_frame = rf_path



func height_source( name: String, radius: float, height: float ):
	var HS = preload( "res://physics/celestial_bodies/height_sources.gd" )
	var hs: HeightSourceRef = HS.height_source( name, radius, height )
	return hs



# Callback on mesh updated.
func on_mesh_updated():
	print( "on \"mesh_updated()\" called" )
	
	var cube_sphere: CubeSphereNode = get_node( "Rotation/CelestialBody" )
	var collision_dist = Constants.RF_MERGE_DISTANCE
	# In all RefFramePhysics re-generate surface meshes.
	var translation: RefFrameNode = get_node( "." ) as RefFrameNode
	var children: Array = translation.get_children()
	for ch in children:
		var rf: RefFramePhysics = ch as RefFramePhysics
		if rf != null:
			var ref_frames = [ rf ]
			var verts: PoolVector3Array = cube_sphere.collision_triangles( rf, ref_frames, collision_dist )
			rf.set_surface_vertices( verts )








