extends ImmediateGeometry


export(Color) var color = Color( 1.0, 0.0, 0.0, 1.0 )
export(Resource) var material
export(int) var pts_qty = 32

# Need to specify these two in order to draw.
var ref_frame: RefFrameNode = null
var motion: CelestialMotionRef = null


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func draw():
	if ref_frame == null:
		return
	if motion == null:
		return
	
	var m: SpatialMaterial = material as SpatialMaterial
	m.albedo_color = color
	
	var player_rf: RefFrame = PhysicsManager.get_player_ref_frame()
	var own_rf: RefFrame = ref_frame
	var scaler: DistanceScalerRef = PhysicsManager.distance_scaler
	var camera: Camera = PhysicsManager.camera
	
	var pts: PoolVector3Array = motion.orbit_points( own_rf, player_rf, camera, scaler, pts_qty )
	
	clear()
	begin(Mesh.PRIMITIVE_LINE_STRIP)
	
	for pt in pts:
		add_vertex( pt )
	
	end()
