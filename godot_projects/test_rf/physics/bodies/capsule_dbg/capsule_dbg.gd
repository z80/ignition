
extends Body

const FORCE_H: float = 30.0
const FORCE_V: float = 100.0

const ANG_VEL: float = 30.0 / 180.0 * 3.14

var _ang_vel: float = 0.0

func _ready():
	init()


func init():
	var Visual   = preload( "res://physics/bodies/capsule_dbg/visual.tscn")
	var Physical = preload( "res://physics/bodies/capsule_dbg/physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()
	
	
	var t: Transform
	t.origin = Vector3( 0.0, 20.0, 0.0 )
	set_t( t )


func update_physical( delta: float ):
	if _physical == null:
		return
	
	if _ang_vel == 0.0:
		return
	
	var t: Transform = _physical.transform
	var a2: float = _ang_vel * delta * 0.5
	var si2 = a2
	var co2 = 1.0 - a2*a2*0.5
	var dq: Quat = Quat( 0.0, si2, 0.0, co2 )
	var q: Quat = t.basis
	q = q * dq
	q = q.normalized()
	t.basis = q
	
	var v: Vector3 = _physical.linear_velocity
	_physical.transform = t
	_physical.linear_velocity = v



func set_collision_layer( layer ):
	if _physical:
		var rb = _physical.get_child( 0 )
		rb.collision_layer = layer
		rb.collision_mask  = layer


func process_user_input( _event: InputEvent ):
	if _physical == null:
		return
	
	_user_input_translation( _event )
	_user_input_rotation( _event )



func _user_input_translation( _event: InputEvent ):
	# For physical to be able to know universe position/rotation.
	_physical.body = self
	var is_w: bool = Input.is_action_pressed("ui_w")
	var is_s: bool = Input.is_action_pressed("ui_s")
	var is_a: bool = Input.is_action_pressed("ui_a")
	var is_d: bool = Input.is_action_pressed("ui_d")
	var is_q: bool = Input.is_action_pressed("ui_q")
	var is_e: bool = Input.is_action_pressed("ui_e")
	
	var f: Vector3 = Vector3.ZERO
	if is_w:
		f = f - Vector3( 0.0, 0.0, FORCE_H )
	if is_s:
		f = f + Vector3( 0.0, 0.0, FORCE_H )
	if is_a:
		f = f - Vector3( FORCE_H, 0.0, 0.0 )
	if is_d:
		f = f + Vector3( FORCE_H, 0.0, 0.0 )
	
	# Vertical force
	if is_e:
		f = f + Vector3( 0.0, FORCE_V, 0.0 )
	if is_q:
		f = f - Vector3( 0.0, FORCE_V, 0.0 )
	
	_physical.apply_force( f )
	
	var mag = f.length()
	if mag > 0.5:
		print( "force applied: ", mag )
	



func _user_input_rotation( _event: InputEvent ):
	var is_j: bool = Input.is_action_pressed( "ui_j" )
	var is_l: bool = Input.is_action_pressed( "ui_l" )
	
	
	var w: float = 0.0
	if is_j:
		w += ANG_VEL
	if is_l:
		w -= ANG_VEL
	
	_ang_vel = w

