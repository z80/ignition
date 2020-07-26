extends Control


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
var mm_ = null
var mouse_over_: bool = false
var mouse_pressed_: bool = false

# Called when the node enters the scene tree for the first time.
func _ready():
	get_mm()
	
	var panel = $Panel
	if not panel:
		return
	panel.connect("mouse_entered", self, "_panel_entered")
	panel.connect("mouse_exited", self, "_panel_exited")
	
	read_gains_from_mm_()



# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if not mm_:
		return
		
	var stri: String = "%d" % mm_.frame_ind_
	$Panel/FrameN.text = stri
	
	var q: Quat = mm_.pose_q_;
	var r: Vector3 = mm_.pose_r_
	
	stri = "(%f, %f, %f)" %[ r.x, r.y, r.z ]
	$Panel/PoseR.text = stri

	r = mm_.pose_dr_
	stri = "(%f, %f, %f)" %[ r.x, r.y, r.z ]
	$Panel/PoseDR.text = stri

	stri = "(%f, %f, %f, %f)" %[ q.w, q.x, q.y, q.z ]
	$Panel/PoseQ.text = stri

	q = mm_.pose_dq_;
	stri = "(%f, %f, %f, %f)" %[ q.w, q.x, q.y, q.z ]
	$Panel/PoseDQ.text = stri

	q = mm_.cam_q_;
	stri = "(%f, %f, %f, %f)" %[ q.w, q.x, q.y, q.z ]
	$Panel/CamQ.text = stri

	q = mm_.cam_rel_q_;
	stri = "(%f, %f, %f, %f)" %[ q.w, q.x, q.y, q.z ]
	$Panel/CamRelQ.text = stri

	var v2: Vector2 = mm_.control_input_;
	stri = "(%f, %f)" %[ v2.x, v2.y ]
	$Panel/InstantCtrl.text = stri
	
	var all_stri: String = ""
	var qty: int = mm_.print_control_sequence_.size()
	for i in range( qty ):
		var c: Vector2 = mm_.print_control_sequence_[i]
		stri = " (%f, %f)" % [c.x, c.y]
		all_stri = all_stri + stri
	$Panel/CtrlSeq.text = all_stri

func _input(event):
	
	if event is InputEventMouseButton and event.button_index == BUTTON_LEFT:
		mouse_pressed_ = event.pressed
	
	elif event is InputEventMouseMotion and mouse_over_ and mouse_pressed_:
		var panel = $Panel
		var sz = panel.rect_size
		panel.set_begin(panel.get_begin() + event.relative)
		panel.rect_size = sz


func get_mm():
	var p = get_parent()
	mm_ = p


func _panel_entered():
	mouse_over_ = true


func _panel_exited():
	mouse_over_ = false


func read_gains_from_mm_():
	var gains = mm_.get_desc_gains()
	
	var qty = gains.size()
	if qty > 2:
		var pose_vel  = gains[1]
		#var pose_g    = gains[2]
		var traj_pos  = gains[2]
		#var traj_az   = gains[4]
		#var traj_g    = gains[5]
	
		var switch_th = mm_.get_switch_threshold()
	
		$Panel/WeightPoseVel.text = String( pose_vel )
		#$Panel/WeightPoseG.text   = String( pose_g )
		$Panel/WeightTrajPos.text = String( traj_pos )
		#$Panel/WeightTrajAz.text  = String( traj_az )
		#$Panel/WeightTrajG.text   = String( traj_g )
	
		$Panel/SwitchTh.text      = String( switch_th )


func write_gains_to_mm_():
	var pose_vel = float( $Panel/WeightPoseVel.text )
	#var pose_g   = float( $Panel/WeightPoseG.text )
	var traj_pos = float( $Panel/WeightTrajPos.text )
	#var traj_az  = float( $Panel/WeightTrajAz.text )
	#var traj_g   = float( $Panel/WeightTrajG.text )
	
	var switch_th = float( $Panel/SwitchTh.text )
	
	#var gains = [ 1.0, pose_vel, pose_g, traj_pos, traj_az, traj_g ]
	var gains = [ 1.0, pose_vel, traj_pos ]
	mm_.set_desc_gains( gains )
	
	mm_.set_switch_threshold( switch_th )


func _on_apply_weights_pressed():
	write_gains_to_mm_()








func _on_get_pressed():
	read_gains_from_mm_()
