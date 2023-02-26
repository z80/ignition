extends ImmediateGeometry


export(Color) var color = Color( 1.0, 0.0, 0.0, 1.0 )
export(Material) var material
export(int) var pts_qty = 128

# Need to specify these two in order to draw.
var ref_frame: RefFrameNode = null
var motion  = null


# Called when the node enters the scene tree for the first time.
#func _ready():
#	self.material_override = material


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	self.material_override = material



func draw():
	# Disable it for now.
	return
	
	if ref_frame == null:
		return
	if motion == null:
		return
	
	var m: SpatialMaterial = material as SpatialMaterial
	m.albedo_color = color
	
	var orbiting_center_rf: RefFrameNode = ref_frame
	var camera_rf: RefFrameNode          = RootScene.ref_frame_root.player_camera
	var scaler: DistanceScalerRef        = RootScene.ref_frame_root.distance_scaler
	
	var pts: PoolVector3Array = motion.orbit_points( orbiting_center_rf, camera_rf, null, scaler, pts_qty )
	
	clear()
	begin(Mesh.PRIMITIVE_LINE_STRIP)
	
	for pt in pts:
		set_color( color )
		add_vertex( pt )
	end()
