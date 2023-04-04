
extends MeshInstance3D

@export var config_detail_level: Resource = null
@export var color: Color = Color( 1.0, 0.0, 0.0, 1.0 )
@export var material: Material
@export var pts_qty: int = 128

var _scale_distance_ratio: ScaleDistanceRatioGd = null

# Called when the node enters the scene tree for the first time.
func _ready():
	_scale_distance_ratio = ScaleDistanceRatioGd.new()
	_scale_distance_ratio.max_distance = config_detail_level.event_horizon


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(_delta):
#	self.material_override = material



func draw( position: RefFrameMotionNode, motion: CelestialMotionRef = null ):
	var m: StandardMaterial3D = material as StandardMaterial3D
	m.albedo_color = color
	
	var camera_rf: RefFrameNode = RootScene.ref_frame_root.player_camera
	var base_scale: float       = 1.0 / config_detail_level.scale_divider
	var pts: PackedVector3Array
	if motion == null:
		pts = position.orbit_points( camera_rf, pts_qty, _scale_distance_ratio, base_scale )
	
	else:
		pts = motion.orbit_points( position, camera_rf, pts_qty, _scale_distance_ratio, base_scale )
	
	var im: ImmediateMesh = ImmediateMesh.new()
	im.surface_begin(Mesh.PRIMITIVE_LINES, m )
	
	for pt in pts:
		im.surface_add_vertex( pt )
	im.surface_end()
	
	self.mesh = im




