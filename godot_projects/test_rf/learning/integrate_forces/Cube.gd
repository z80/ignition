extends RigidBody

const SPEED = 10.0
const GAIN = 50.5

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

func _integrate_forces(state):
	var dt: float = state.step
	var w: bool = Input.is_action_pressed( "ui_w" )
	var s: bool = Input.is_action_pressed( "ui_s" )
	var a: bool = Input.is_action_pressed( "ui_a" )
	var d: bool = Input.is_action_pressed( "ui_d" )
	var v: Vector3 = Vector3.ZERO
	if w:
		v += Vector3.FORWARD
	if s:
		v += Vector3.BACK
	if a:
		v += Vector3.LEFT
	if d:
		v += Vector3.RIGHT
	if v.length_squared() > 0.0:
		v = v.normalized()
		v *= SPEED
	
	var model_v: Vector3 = self.linear_velocity
	var dv: Vector3 = v - model_v
	var f: Vector3 = GAIN * dv
	state.add_central_force( f )
	#self.add_central_force( f )
