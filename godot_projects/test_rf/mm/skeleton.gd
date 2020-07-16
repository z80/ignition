extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
var root_bone_index: int = -1
var time: float = 0.0
const PERIOD: float = 3.0

# Called when the node enters the scene tree for the first time.
func _ready():
	var s: Skeleton = $Skeleton
	var qty: int = s.get_bone_count()
	root_bone_index = s.find_bone( "Hips" )

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	time += delta
	if time >= PERIOD:
		time -= PERIOD
	
	var s: Skeleton = $Skeleton
	var qty: int = s.get_bone_count()
	var t: Transform = s.get_bone_pose( root_bone_index )
	var x: float = 2.0 * sin( 6.28 * time/PERIOD )
	var r = t.origin
	r[0] = x
	t.origin = r
	s.set_bone_pose( root_bone_index, t )
	



