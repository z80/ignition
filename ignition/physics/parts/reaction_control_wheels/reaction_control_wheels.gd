
extends Part
class_name ReactionControlWheels

export(float) var max_torque       = 50.0
export(float) var angualr_velocity = 1.0
export(float) var angular_gain     = 1000.0

var _user_input: Dictionary = {}
var _w_normalized: Vector3 = Vector3.ZERO
var _torque: Vector3       = Vector3.ZERO

var _control_applied: bool = false
export(String) var sound: String = ""


func init():
	.init()
	
	var PartControlGroups = load( "res://physics/parts/part_control_groups.gd" )
	control_group = PartControlGroups.ControlGroup._1



func process_user_input_2(input: Dictionary ):
	_user_input = input.duplicate( true )



func _physics_process( _delta ):
	._physics_process( _delta )
	apply_control( _delta )



# This one should be called from inside of a physics step.
func apply_control( dt: float ):
	if _physical == null:
		return

	var w: Vector3 = Vector3.ZERO
	
	var i: bool   = _user_input.has( "ui_i" )
	var k: bool   = _user_input.has( "ui_k" )
	var j: bool   = _user_input.has( "ui_j" )
	var l: bool   = _user_input.has( "ui_l" )
	var u: bool   = _user_input.has( "ui_u" )
	var o: bool   = _user_input.has( "ui_o" )
	var sas: bool = _user_input.has( "gui_sas" )
	
	var apply_ctrl: bool = false
	if i:
		w += Vector3.LEFT
	if k:
		w += Vector3.RIGHT
	if j:
		w += Vector3.BACK
	if l:
		w += Vector3.FORWARD
	if u:
		w += Vector3.DOWN
	if o:
		w += Vector3.UP
	
	if w.length_squared() > 0.0:
		w = w.normalized()
		apply_ctrl = true
	
	if apply_ctrl and (not _control_applied):
		play_sound( sound )
	elif (not apply_ctrl) and _control_applied:
		stop_sound( sound )
	
	_control_applied = apply_ctrl
	
	if (not sas) and (not _control_applied):
		return
	
	_w_normalized = w
	_torque       = Vector3.ZERO
	
	# Adjustment q.
	var t: Transform = self.transform
	var q: Quat      = t.basis
	
	var wanted_w = w
	#if not rotation_abolute:
	wanted_w = q.xform( wanted_w )
	wanted_w *= angualr_velocity
	var current_w: Vector3 = self.w()
	var dw = wanted_w - current_w
	var torque: Vector3 = dt * angular_gain * dw
	var L = torque.length()
	
	if L > max_torque:
		torque = torque * (max_torque / L)
	
	_torque = torque
	_physical.add_torque( _torque )







