extends Camera

const State = {
	STATE_TPS_AZIMUTH = 0,
	STATE_TPS_FREE = 1, 
	STATE_FPS = 2
}

var _state: int = State.STATE_TPS_AZIMUTH
var _mouse_displacement: Vector2 = Vector2.ZERO

export(int) var state setget _set_state
export(float) var dist_min = 1.0   setget _set_dist_min
export(float) var dist_max = 100.0 setget _set_dist_max

export(NodePath) var target_path_tps setget _set_target_path_tps
export(NodePath) var target_path_fps setget _set_target_path_fps


var _target_tps: Spatial = null
var _target_fps: Spatial = null

var _dist_tps: float = 5.0

func _set_state( st ):
	state = st

func _set_dist_min( v ):
	dist_min = v

func _set_dist_max( v ):
	dist_max = v

func _set_target_path_tps( pth ):
	target_path_tps = pth
	_target_tps = get_node( target_path_tps )

func _set_target_path_fps( pth ):
	target_path_fps = pth
	_target_fps = get_node( target_path_fps )


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _input( event ):
	if event is InputEventMouseMotion:
		_mouse_displacement = event.relative

