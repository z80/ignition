extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
var _db = null

var _frame_ind: int = 0
const FPS: float = 24.0
var _period: float = 1.0/FPS
var _elapsed: float = 0.0
var _frame: Array = []

# Called when the node enters the scene tree for the first time.
func _ready():
	_db = $FramesDb
	_db.open()
	
	_frame_ind = 0
	_elapsed = 0.0
	_period = 1.0/FPS
	


func _exit_tree():
	if _db != null:
		_db.close()


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	$Vp.set_camera_pose( $Camera.transform )
	
	_elapsed += delta
	if _elapsed >= _period:
		_elapsed -= _period
		_frame_ind += 1
		_frame = _db.get_frame( _frame_ind )
		$Vp.set_frame( _frame )

	if _frame.size() > 0:
		var mmv = $MotionMatchingVive
		mmv.generate_controls( _frame )
		
		var f = mmv._f
		$Dn.set_frame( f )


