extends Control


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
var mm_ = null
var mouse_over_: bool = false
var mouse_pressed_: bool = false

# For recording sequences
var _mm_saver: MmSaver = MmSaver.new()
var _mm_save_sequence: bool = false



# Called when the node enters the scene tree for the first time.
func _ready():
	get_mm()
	
	var panel = $Panel
	if not panel:
		return
	panel.connect("mouse_entered", self, "_panel_entered")
	panel.connect("mouse_exited", self, "_panel_exited")
	
	_on_ReadBtn_pressed()
	



# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if not mm_:
		return
	
	if _mm_save_sequence:
		_mm_saver.store( mm_, mm_.f_, mm_.frame_ind_ )
		
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
	
	$"Panel/Tabs/Motion labeling/CurFrameLbl".text = String( mm_.frame_ind_ )
	

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
	if qty > 6:
		var pose_vel  = gains[1]
		var pose_cat  = gains[2]
		var traj_pos  = gains[3]
		var traj_z    = gains[4]
		var traj_hd   = gains[5]
		var traj_cat  = gains[6]
	
		var switch_th = mm_.get_switch_threshold()
	
		$Panel/Tabs/Weights/WeightPoseVel.text = String( pose_vel )
		$Panel/Tabs/Weights/WeightPoseCat.text = String( pose_cat )
		$Panel/Tabs/Weights/WeightTrajPos.text = String( traj_pos )
		$Panel/Tabs/Weights/WeightTrajZ.text   = String( traj_z )
		$Panel/Tabs/Weights/WeightTrajHeading.text   = String( traj_hd )
		$Panel/Tabs/Weights/WeightTrajCat.text = String( traj_cat )
	
		$Panel/Tabs/Weights/SwitchTh.text      = String( switch_th )


func write_gains_to_mm_():
	var pose_vel = float( $Panel/Tabs/Weights/WeightPoseVel.text )
	var pose_cat = float( $Panel/Tabs/Weights/WeightPoseCat.text )
	var traj_pos = float( $Panel/Tabs/Weights/WeightTrajPos.text )
	var traj_z   = float( $Panel/Tabs/Weights/WeightTrajZ.text )
	var traj_hd  = float( $Panel/Tabs/Weights/WeightTrajHeading.text )
	var traj_cat = float( $Panel/Tabs/Weights/WeightTrajCat.text )
	
	var switch_th = float( $Panel/Tabs/Weights/SwitchTh.text )
	
	#var gains = [ 1.0, pose_vel, pose_g, traj_pos, traj_az, traj_g ]
	var gains = [ 1.0, pose_vel, pose_cat, traj_pos, traj_z, traj_hd, traj_cat ]
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
	if mm_.frame_ind_ > 0:
		mm_.frame_ind_ -= 1


func _on_NextBtn_pressed():
	if mm_.frame_ind_ < (mm_.frames_qty_-1):
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


func _on_InstantDataBtn_pressed():
	var d = mm_.desc_( mm_.db_, mm_.frame_ind_ )
	print( "desc: ", d )


func _on_ComputeDescBtn_pressed():
	var pose_desc = mm_.pose_desc_( mm_.db_, mm_.frame_ind_ )
	var traj_desc = mm_.traj_desc_( mm_.db_, mm_.frame_ind_ )
	var gen_pose_desc = mm_.input_based_pose_desc_( mm_.db_, mm_.frame_ind_ )
	var gen_traj_desc = mm_.input_based_traj_desc_( mm_.db_ )
	print( "\n\n\n" )
	print( "frame # ", mm_.frame_ind_ )
	print( "Current one:" )
	print( "pose desc:     ", pose_desc )
	print( "traj desc:     ", traj_desc )
	print( "Wanted one:" )
	print( "gen pose desc: ", gen_pose_desc )
	print( "gen traj desc: ", gen_traj_desc )
	
	var d = []
	for di in gen_pose_desc:
		for v in di:
			d.push_back( v )
	for di in gen_traj_desc:
		for v in di:
			d.push_back( v )
	var closest_err: float = mm_.frame_search_.nearest( d )
	var closest_ind: int   = mm_.frame_search_.nearest_ind()

	var next_err: float = mm_.frame_search_.dist( d, mm_.frame_ind_ )
	var improvement: float = next_err - closest_err
	
	var cl_pose_desc = mm_.pose_desc_( mm_.db_, closest_ind )
	var cl_traj_desc = mm_.traj_desc_( mm_.db_, closest_ind )
	print( "Closest one:" )
	print( "cl pose desc:  ", cl_pose_desc )
	print( "cl traj desc:  ", cl_traj_desc )
	
	print( "closest err: ", closest_err )
	print( "next err:    ", next_err )
	print( "improvement: ", improvement )
	print( "closest frame #: ", closest_ind )
	
	var min_err: float = -1.0
	var min_ind: int = -1
	for i in range ( mm_.frames_qty_ ):
		var err: float = mm_.frame_search_.dist( d, i )
		if min_err < 0.0 or err < min_err:
			min_err = err
			min_ind = i
	
	# Manually comute the distance.
	var gains = mm_.get_desc_gains()
	print( "Gains: ", gains )
	var desc_weights = mm_.desc_weights_
	var weights = []
	var diff = []
	var abs_diff = 0.0
	var gain_ind = 0
	var weight_ind = 0
	var pose_sz = gen_pose_desc.size()
	var traj_sz = gen_traj_desc.size()
	
	for i in range( pose_sz ):
		var fr = cl_pose_desc[i]
		var gen = gen_pose_desc[i]
		var di = []
		var g = gains[gain_ind]
		var pose_sz_i = fr.size()
		for j in range( pose_sz_i ):
			var gain = desc_weights[weight_ind] * g
			weights.push_back( gain )
			var d2 = fr[j] - gen[j]
			d2 = d2 * d2 * gain
			di.push_back( d2 )
			abs_diff += d2
			weight_ind += 1
		diff.push_back( di )
		gain_ind += 1
	for i in range( traj_sz ):
		var fr = cl_traj_desc[i]
		var gen = gen_traj_desc[i]
		var di = []
		var g = gains[gain_ind]
		var traj_sz_i = fr.size()
		for j in range( traj_sz_i ):
			var gain = desc_weights[weight_ind] * g
			weights.push_back( gain )
			var d2 = fr[j] - gen[j]
			d2 = d2 * d2 * gain
			di.push_back( d2 )
			abs_diff += d2
			weight_ind += 1
		diff.push_back( di )
		gain_ind += 1
	
	print( "diff: ", diff )
	print( "abs_diff: ", abs_diff )
	var read_weights = mm_.frame_search_.weights()
	print( "read from frame search weights: ", read_weights )
	print( "computed weights:               ", weights )
	
	print( "brute force best ind: ", min_ind, "; best err: ", min_err )







func _on_CaptureBtn_pressed():
	
	var en: bool = _mm_save_sequence
	if en:
		_mm_saver.close()
	else:
		var names_map = processBoneCorrespondences()
		_mm_saver.set_bone_names( mm_.bone_names_, names_map )
		_mm_saver.init()
	_mm_save_sequence = not _mm_save_sequence


func _on_ReadBtn_pressed():
	var file = File.new()
	file.open("res://bone_correspondence.txt", File.READ)
	var content: String = file.get_as_text()
	file.close()
	$Panel/Tabs/Capture/BoneCorrespondences.text = content


func _on_SaveBtn_pressed():
	var content: String = $Panel/Tabs/Capture/BoneCorrespondences.text
	var file = File.new()
	file.open("res://bone_correspondence.txt", File.WRITE)
	file.store_string( content )
	file.close()



func processBoneCorrespondences():
	var names_map: Dictionary = {}
	
	var regex = RegEx.new()
	regex.compile("(?<src_name>\\S+)\\s*\\->\\s*(?<dest_name>\\S+)")
	var text: String = $Panel/Tabs/Capture/BoneCorrespondences.text
	for m in regex.search_all( text ):
		var src_name: String  = m.get_string( "src_name" )
		var dest_name: String = m.get_string( "dest_name" )
		names_map[src_name] = dest_name
	
	return names_map







func _on_PrintQuatsBtn_pressed():
	var regex = RegEx.new()
	regex.compile("(?<src_name>\\S+)\\s*\\->\\s*(?<dest_name>\\S+)")
	var text: String = $Panel/Tabs/Capture/BoneCorrespondences.text
	for m in regex.search_all( text ):
		var src_name: String  = m.get_string( "src_name" )
		var dest_name: String = m.get_string( "dest_name" )
