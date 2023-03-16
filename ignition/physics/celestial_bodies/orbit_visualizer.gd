extends ImmediateGeometry


export(Resource) var config_detail_level = preload( "res://tryouts/celestial_bodies_voxel_2/config/config_detail_level.tres" )
export(Color) var color = Color( 1.0, 0.0, 0.0, 1.0 )
export(Material) var material
export(int) var pts_qty = 128

var _scale_distance_ratio: ScaleDistanceRatioGd = null

# Called when the node enters the scene tree for the first time.
func _ready():
	_scale_distance_ratio = ScaleDistanceRatioGd.new()
	_scale_distance_ratio.max_distance = config_detail_level.event_horizon


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	self.material_override = material



func draw( translation: RefFrameMotionNode ):
	var m: SpatialMaterial = material as SpatialMaterial
	m.albedo_color = color
	
	var camera_rf: RefFrameNode = RootScene.ref_frame_root.player_camera
	var base_scale: float       = 1.0 / config_detail_level.scale_divider
	var pts: PoolVector3Array = translation.orbit_points( camera_rf, pts_qty, _scale_distance_ratio, base_scale )
	
	clear()
	begin(Mesh.PRIMITIVE_LINE_STRIP)
	
	for pt in pts:
		set_color( color )
		add_vertex( pt )
	end()
