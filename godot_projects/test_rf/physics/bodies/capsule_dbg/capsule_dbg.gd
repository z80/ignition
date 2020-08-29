
extends Body

const FORCE_H: float = 30.0
const FORCE_V: float = 100.0

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



func set_collision_layer( layer ):
	if _physical:
		var rb = _physical.get_child( 0 )
		rb.collision_layer = layer
		rb.collision_mask  = layer


func process_user_input( _event: InputEvent ):
	if _physical == null:
		return
	
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
	




