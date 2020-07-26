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
	
	#read_gains_from_mm_()



# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if not mm_:
		return
		
	var stri: String = "%d" % mm_.frame_ind_
	$Panel/Tabs/Weights/FrameN.text = stri
	
	var q: Quat = mm_.pose_q_;
	var r: Vector3 = mm_.pose_r_
	
	stri = "(%f, %f, %f)" %[ r.x, r.y, r.z ]
	$Panel/Tabs/Weights/PoseR.text = stri

	r = mm_.pose_dr_
	stri = "(%f, %f, %f)" %[ r.x, r.y, r.z ]
	$Panel/Tabs/Weights/PoseDR.text = stri

	stri = "(%f, %f, %f, %f)" %[ q.w, q.x, q.y, q.z ]
	$Panel/Tabs/Weights/PoseQ.text = stri

	q = mm_.pose_dq_;
	stri = "(%f, %f, %f, %f)" %[ q.w, q.x, q.y, q.z ]
	$Panel/Tabs/Weights/PoseDQ.text = stri

	q = mm_.cam_q_;
	stri = "(%f, %f, %f, %f)" %[ q.w, q.x, q.y, q.z ]
	$Panel/Tabs/Weights/CamQ.text = stri

	q = mm_.cam_rel_q_;
	stri = "(%f, %f, %f, %f)" %[ q.w, q.x, q.y, q.z ]
	$Panel/Tabs/Weights/CamRelQ.text = stri

	var v2: Vector2 = mm_.control_input_;
	stri = "(%f, %f)" %[ v2.x, v2.y ]
	$Panel/Tabs/Weights/InstantCtrl.text = stri
	
	var all_stri: String = ""
	var qty: int = mm_.print_control_sequence_.size()
	for i in range( qty ):
		var c: Vector2 = mm_.print_control_sequence_[i]
		stri = " (%f, %f)" % [c.x, c.y]
		all_stri = all_stri + stri
	$Panel/Tabs/Weights/CtrlSeq.text = all_stri

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
	if qty > 3:
		var pose_vel  = gains[1]
		#var pose_g    = gains[2]
		var traj_pos  = gains[2]
		#var traj_az   = gains[4]
		var traj_g    = gains[3]
	
		var switch_th = mm_.get_switch_threshold()
	
		$Panel/Tabs/Weights/WeightPoseVel.text = String( pose_vel )
		#$Panel/WeightPoseG.text   = String( pose_g )
		$Panel/Tabs/Weights/WeightTrajPos.text = String( traj_pos )
		#$Panel/WeightTrajAz.text  = String( traj_az )
		$Panel/Tabs/Weights/WeightTrajG.text   = String( traj_g )
	
		$Panel/Tabs/Weights/SwitchTh.text      = String( switch_th )


func write_gains_to_mm_():
	var pose_vel = float( $Panel/Tabs/Weights/WeightPoseVel.text )
	#var pose_g   = float( $Panel/WeightPoseG.text )
	var traj_pos = float( $Panel/Tabs/Weights/WeightTrajPos.text )
	#var traj_az  = float( $Panel/WeightTrajAz.text )
	var traj_g   = float( $Panel/Tabs/Weights/WeightTrajG.text )
	
	var switch_th = float( $Panel/Tabs/Weights/SwitchTh.text )
	
	#var gains = [ 1.0, pose_vel, pose_g, traj_pos, traj_az, traj_g ]
	var gains = [ 1.0, pose_vel, traj_pos, traj_g ]
	mm_.set_desc_gains( gains )
	
	mm_.set_switch_threshold( switch_th )


func _on_apply_weights_pressed():
	write_gains_to_mm_()








func _on_get_pressed():
	read_gains_from_mm_()


func _on_PlayStopBtn_pressed():
	mm_.increment_frame_ind_ = not mm_.increment_frame_ind_





func _on_ToPlaybackBtn_pressed():
	mm_.run_mm_algorithm_ = false


func _on_ToMmBtn_pressed():
	mm_.run_mm_algorithm_ = true


func _on_GoToFrameBtn_pressed():
	var stri: String = $"Panel/Tabs/Motion labeling/GoToEdt".text
	var ind = int(stri)
	mm_.frame_ind_ = ind
	


func _on_PrevBtn_pressed():
	mm_.frame_ind_ -= 1


func _on_NextBtn_pressed():
	mm_.frame_ind_ += 1







func _on_SetFromBtn_pressed():
	var ind = mm_.frame_ind_
	$"Panel/Tabs/Motion labeling/FromLbl".text = String(ind)
	


func _on_SetToBtn_pressed():
	var ind = mm_.frame_ind_
	$"Panel/Tabs/Motion labeling/ToLbl".text = String(ind)


func _on_AssignCat_pressed():
	var from_s = $"Panel/Tabs/Motion labeling/FromLbl".text
	var to_s   = $"Panel/Tabs/Motion labeling/ToLbl".text
	var cat_s  = $"Panel/Tabs/Motion labeling/Cat".text
	var from = int(from_s)
	var to   = int(to_s)
	var cat  = int(cat_s)
	mm_.assign_cat( from, to, cat )
