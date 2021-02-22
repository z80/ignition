
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

# As currently there is just one player
# And even if many one player per PC need just one 
# subdivide source per visual sphere.
var _subdivide_source_visual: SubdivideSourceRef = null


# Called when the node enters the scene tree for the first time.
func _ready():
	init()


func init_force_source():
	.init_force_source()
	force_source = ForceSourceGravity.new()


func translation_rf():
	var rf: RefFrameNode = get_node( "." ) as RefFrameNode
	return rf


func rotation_rf():
	var rf: RefFrameNode = get_node( "Rotation" ) as RefFrameNode
	return rf


func surface_node():
	var cube_sphere: CubeSphereNode = get_node( "Rotation/CelestialBody" ) as CubeSphereNode
	return cube_sphere




func init():
	if initialized:
		return
	initialized = true
	
	add_to_group( Constants.SPHERES_GROUP_NAME )
	
	_subdivide_source_visual = SubdivideSourceRef.new()
	
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
	celestial_body.convert_to_global   = true
	celestial_body.scale_mode_distance = 3.0
	
	celestial_body.clear_levels()
	celestial_body.add_level( detail_size_0, detail_dist_0 )
	celestial_body.add_level( detail_size_1, detail_dist_1 )
	celestial_body.add_level( detail_size_2, detail_dist_2 )
	
	#connect( "mesh_updated", self, "on_mesh_updated" )



func process( delta ):
	process_motion( delta )
	process_geometry()



func process_motion( delta ):
	var translation_rf = get_node( "." )
	var rotation_rf    = get_node( "Rotation" )
	motion.process_rf( delta, translation_rf )
	rotation.process_rf( delta, rotation_rf )
	
	var t: Transform = rotation_rf.transform
	#print( "rotation: ", Quat( t.basis ) )



func process_geometry():
	var player_rf: RefFramePhysics = PhysicsManager.player_ref_frame
	var physics_ref_frames: Dictionary  = PhysicsManager.physics_ref_frames()
	
	var translation: RefFrameNode = self
	var rotation: RefFrameNode    = get_node( "Rotation" )
	var planet: CubeSphereNode = get_node( "Rotation/CelestialBody" )
	
	var paths: Array = []
	for k in physics_ref_frames:
		var rf: RefFramePhysics = physics_ref_frames[k]
		# Check if either node is direct parent of this rf
		var p = rf.get_parent()
		var is_child: bool = ( p == planet ) or ( p == rotation ) or ( p == translation )
		if not is_child:
			continue
		
		var subdiv: SubdivideSourceRef = rf.get_subdivide_source()
		var need_rebuild: bool = subdiv.need_subdivide( rf, planet )
		if need_rebuild:
			# Build the surface for this particular ref frame physics.
			planet.rebuild_shape( rf, subdiv )
			var collision_dist = Constants.RF_MERGE_DISTANCE
			var verts: PoolVector3Array = planet.collision_triangles( rf, subdiv, collision_dist )
			var surface_relative_to_rf: Se3Ref = planet.relative_to( rf )
			rf.set_surface_vertices( verts, surface_relative_to_rf )
		else:
			# Need to update current pose for surface vertices as 
			# planet position might have changed if ref. frame is not 
			# a child of the rotation part.
			#var surface_relative_to_rf: Se3Ref = planet.relative_to( rf )
			#rf.update_surface_vertices( surface_relative_to_rf )
			pass
	
	# For player ref frame rebuild mesh if needed
	if player_rf != null:
		var need_rebuild_visual: bool = _subdivide_source_visual.need_subdivide( player_rf, planet )
		#print( "need_rebuild_visual: ", need_rebuild_visual )
		if need_rebuild_visual:
			planet.rebuild_shape( player_rf, _subdivide_source_visual )
			planet.apply_visual_mesh()
		planet.relocate_mesh( player_rf )










func height_source( name: String, radius: float, height: float ):
	var HS = preload( "res://physics/celestial_bodies/height_sources.gd" )
	var hs: HeightSourceRef = HS.height_source( name, radius, height )
	return hs











