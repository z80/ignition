
extends Node
class_name DraggedRf

const HEAD_FORWARD: Vector3 = Vector3( 0.0, 0.0, -1.0 )


export(float) var min_dist = 0.8
export(float) var max_dist = 1.2

var _initialized: bool = false
var _dragged_at: Vector3
var _dragged_q: Quat

func _init():
	reset()


func _ready():
	pass


func reset():
	_initialized = false


func _initialize_drag( q: Quat, r: Vector3 ):
	var fwd: Vector3 = q.xform( HEAD_FORWARD )
	fwd.y = 0.0
	fwd = fwd / fwd.length()
	fwd = fwd * max_dist
	_dragged_at = r - fwd


func _process_drag( q: Quat, r: Vector3 ):
	var dr: Vector3 = r - _dragged_at
	var d: float = dr.length()
	
	if d > max_dist:
		dr = dr * (max_dist / d)
		_dragged_at = r - dr
		
	elif d < min_dist:
		dr = dr * (min_dist / d)
		_dragged_at = r - dr
	
	_dragged_at.y = 0.0


func _compute_drag_rf( q: Quat, r: Vector3 ):
	var dr: Vector3 = r - _dragged_at
	dr.y = 0.0
	dr = dr / dr.length()
	var Oy: Vector3 = Vector3( 0.0, 1.0, 0.0 )
	var Ox: Vector3 = dr.cross( Oy )
	var Oz: Vector3 = -dr
	var b: Basis
	b.x = Ox
	b.y = Oy
	b.z = Oz
	_dragged_q = b
	
	# For debugging:
	#var v: Vector3 = Vector3( 0.0, 0.0, -1.0 )
	#v = _dragged_q.xform( v )
	#print( "forward v: ", v )



func process( head_pose: Array ):
	var q: Quat = Quat( head_pose[1], head_pose[2], head_pose[3], head_pose[0] )
	var r: Vector3 = Vector3( head_pose[4], 0.0, head_pose[6] )
	
	if not _initialized:
		_initialize_drag( q, r )
		_initialized = true
		
	else:
		_process_drag( q, r )
	
	_compute_drag_rf( q, r )


func position():
	return _dragged_at


func rotation():
	return _dragged_q

