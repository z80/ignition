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
	return
	
	time += delta
	if time >= PERIOD:
		time -= PERIOD
	
	var s: Skeleton = $Skeleton
	var qty: int = s.get_bone_count()
	var t: Transform = s.get_bone_pose( _root_bone_index )
	var x: float = 2.0 * sin( 6.28 * time/PERIOD )
	var r = t.origin
	r[0] = x
	t.origin = r
	s.set_bone_pose( _root_bone_index, t )
	

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




