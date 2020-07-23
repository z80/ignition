extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
var time: float = 0.0
const PERIOD: float = 3.0

var _root_bone_index: int = -1
var _name_to_index = {}

# Called when the node enters the scene tree for the first time.
func _ready():
	prepare_name_to_index_dict()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var t: Transform = $Camera.get_camera_transform()
	var mm = $Skeleton/MotionMatching
	var w: bool = Input.is_action_pressed( "walk_w" )
	var s: bool = Input.is_action_pressed( "walk_s" )
	var a: bool = Input.is_action_pressed( "walk_a" )
	var d: bool = Input.is_action_pressed( "walk_d" )
	var fast: bool = Input.is_action_pressed( "walk_fast" )
	var slow: bool = Input.is_action_pressed( "walk_slow" )
	mm.generate_controls( t, w, s, a, d, fast, slow )
	
	mm.time_passed_ += delta
	while ( mm.time_passed_ >= mm.DT ):
		mm.process_frame()
		mm.time_passed_ -= mm.DT












	

func prepare_name_to_index_dict():
	var s: Skeleton = $Skeleton
	var qty: int = s.get_bone_count()
	_root_bone_index = s.find_bone( "Hips" )
	_name_to_index.clear()
	for i in range( qty ):
		var name: String = s.get_bone_name( i )
		_name_to_index[name] = i

func set_bone_t( name: String, t: Transform ):
	var index: int = _name_to_index[name]
	var s: Skeleton = $Skeleton
	s.set_bone_pose( index, t )




