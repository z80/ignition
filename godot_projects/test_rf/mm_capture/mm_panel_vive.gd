extends Control


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
export(NodePath) var mm_path setget set_mm_path
var mm_ = null
var mouse_over_: bool = false
var mouse_pressed_: bool = false

# For recording sequences
var _mm_saver: MmSaver = MmSaver.new()
var _mm_save_sequence: bool = false



# Called when the node enters the scene tree for the first time.
func _ready():
	set_mm_path( mm_path )
	
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
	
	var stri: String = "%d" % mm_._frame_ind
	$Panel/Tabs/Weights/FrameN.text = stri
	
	var q: Quat = mm_._pose_q;
	var r: Vector3 = mm_._pose_r
	
	stri = "(%f, %f, %f)" %[ r.x, r.y, r.z ]
	$Panel/Tabs/Weights/PoseR.text = stri

	r = mm_._pose_dr
	stri = "(%f, %f, %f)" %[ r.x, r.y, r.z ]
	$Panel/Tabs/Weights/PoseDR.text = stri

	stri = "(%f, %f, %f, %f)" %[ q.w, q.x, q.y, q.z ]
	$Panel/Tabs/Weights/PoseQ.text = stri

	q = mm_._pose_dq;
	stri = "(%f, %f, %f, %f)" %[ q.w, q.x, q.y, q.z ]
	$Panel/Tabs/Weights/PoseDQ.text = stri


	#var v2: Vector2 = mm_._control_input;
	#$Panel/Tabs/Weights/InstantCtrl.text = stri
	
	#var all_stri: String = ""
	#var qty: int = mm_.print_control_sequence_.size()
	#for i in range( qty ):
	#	var c: Vector2 = mm_.print_control_sequence_[i]
	#	stri = " (%f, %f)" % [c.x, c.y]
	#	all_stri = all_stri + stri
	#$Panel/Tabs/Weights/CtrlSeq.text = all_stri
	
	$"Panel/Tabs/Motion labeling/CurFrameLbl".text = String( mm_._frame_ind )
	

func _input(event):
	
	if event is InputEventMouseButton and event.button_index == BUTTON_LEFT:
		mouse_pressed_ = event.pressed
	
	elif event is InputEventMouseMotion and mouse_over_ and mouse_pressed_:
		var panel = $Panel
		var sz = panel.rect_size
		panel.set_begin(panel.get_begin() + event.relative)
		panel.rect_size = sz


func set_mm_path( p ):
	mm_path = p
	mm_ = get_node( p )


func _panel_entered():
	mouse_over_ = true


func _panel_exited():
	mouse_over_ = false


func read_gains_from_mm_():
	var gains = mm_.get_desc_gains()
	
	var qty = gains.size()
	if qty > 5:
		var pose_vel  = gains[1]
		var pose_cat  = gains[2]
		var traj_pos  = gains[3]
		var traj_hd   = gains[4]
		var traj_cat  = gains[5]
	
		var switch_th = mm_.get_switch_threshold()
	
		$Panel/Tabs/Weights/WeightPoseVel.text = String( pose_vel )
		$Panel/Tabs/Weights/WeightPoseCat.text = String( pose_cat )
		$Panel/Tabs/Weights/WeightTrajPos.text = String( traj_pos )
		#$Panel/Tabs/Weights/WeightTrajZ.text   = String( traj_z )
		$Panel/Tabs/Weights/WeightTrajHeading.text   = String( traj_hd )
		$Panel/Tabs/Weights/WeightTrajCat.text = String( traj_cat )
	
		$Panel/Tabs/Weights/SwitchTh.text      = String( switch_th )


func write_gains_to_mm_():
	var pose_vel = float( $Panel/Tabs/Weights/WeightPoseVel.text )
	var pose_cat = float( $Panel/Tabs/Weights/WeightPoseCat.text )
	var traj_pos = float( $Panel/Tabs/Weights/WeightTrajPos.text )
	#var traj_z   = float( $Panel/Tabs/Weights/WeightTrajZ.text )
	var traj_hd  = float( $Panel/Tabs/Weights/WeightTrajHeading.text )
	var traj_cat = float( $Panel/Tabs/Weights/WeightTrajCat.text )
	
	var switch_th = float( $Panel/Tabs/Weights/SwitchTh.text )
	
	#var gains = [ 1.0, pose_vel, pose_g, traj_pos, traj_az, traj_g ]
	var gains = [ 1.0, pose_vel, pose_cat, traj_pos, traj_hd, traj_cat ]
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
	var d = mm_._pd.get_desc( mm_._frame_ind )
	print( "desc: ", d )


func _on_ComputeDescBtn_pressed():
	var pose_desc = mm_._pose_desc( mm_._frame_ind )
	var traj_desc = mm_._traj_desc( mm_._frame_ind )
	var gen_pose_desc = mm_._input_based_pose_desc()
	var gen_traj_desc = mm_._input_based_traj_desc()
	print( "\n\n\n" )
	print( "frame # ", mm_._frame_ind )
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
	var closest_err: float = mm_._frame_search.nearest( d )
	var closest_ind: int   = mm_._frame_search.nearest_ind()

	var next_err: float = mm_._frame_search.dist( d, mm_._frame_ind )
	var improvement: float = next_err - closest_err
	
	var cl_pose_desc = mm_._pose_desc( closest_ind )
	var cl_traj_desc = mm_._traj_desc( closest_ind )
	print( "Closest one:" )
	print( "cl pose desc:  ", cl_pose_desc )
	print( "cl traj desc:  ", cl_traj_desc )
	
	print( "closest err: ", closest_err )
	print( "next err:    ", next_err )
	print( "improvement: ", improvement )
	print( "closest frame #: ", closest_ind )
	
	var min_err: float = -1.0
	var min_ind: int = -1
	for i in range ( mm_._frames_qty ):
		var err: float = mm_.frame_search_.dist( d, i )
		if min_err < 0.0 or err < min_err:
			min_err = err
			min_ind = i
	
	# Manually comute the distance.
	var gains = mm_.get_desc_gains()
	print( "Gains: ", gains )
	var desc_weights = mm_._desc_weights
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
	var read_weights = mm_._frame_search.weights()
	print( "read from frame search weights: ", read_weights )
	print( "computed weights:               ", weights )
	
	print( "brute force best ind: ", min_ind, "; best err: ", min_err )







func _on_CaptureBtn_pressed():
	
	var en: bool = _mm_save_sequence
	if en:
		_mm_saver.close()
	else:
		var names_map = _names_map()
		_mm_saver.set_names_and_quats( names_map )
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
	var names_map = {}
	var regex = RegEx.new()
	regex.compile("(?<src_name>\\S+)\\s*\\->\\s*(?<dest_name>\\S+)")
	var text: String = $Panel/Tabs/Capture/BoneCorrespondences.text
	for m in regex.search_all( text ):
		var src_name: String  = m.get_string( "src_name" )
		var dest_name: String = m.get_string( "dest_name" )
		names_map[src_name] = dest_name

	var ind_2_name = {}
	var all_names = mm_.bone_names_
	var names_subset: Array = names_map.keys()
	for name in names_subset:
		if name in all_names:
			var ind: int = all_names.find( name )
			var target_name: String = names_map[name]
			ind_2_name[ind] = { "src": name, "dest": target_name }
	
	# Get current MM display frame.
	# Extract quaternions from there.
	var frame = mm_.f_
	var root_ind = mm_.ROOT_IND * 7
	var root_q = Quat( frame[root_ind+1], frame[root_ind+2], frame[root_ind+3], frame[root_ind] )
	var az_root_q = mm_.quat_azimuth_( root_q )
	
	var fwd = az_root_q.xform( mm_.V_HEADING_FWD )
	var ang: float = atan2( fwd.y, fwd.x )
	var ang_2: float = ang / 2.0
	var co2: float = cos(ang_2)
	var si2: float = sin(ang_2)
	var inv_az_root_q = Quat( 0.0, 0.0, -si2, co2 )
	
	
	for ind in ind_2_name.keys():
		var base = ind * 7
		
		var q: Quat = Quat( frame[base+1], frame[base+2], frame[base+3], frame[base] )
		q = inv_az_root_q * q
		q = q.inverse()
		
		var v = ind_2_name[ind]
		v["q"] = q
		ind_2_name[ind] = v
	
	# Dump to the file.
	var file: File = File.new()
	file.open("res://bone_correspondence.txt", File.WRITE)
	if not file.is_open():
		print( "error opening bone_correspondences file" )
		return
	#file.store_string( content )
	
	for ind in ind_2_name.keys():
		var v = ind_2_name[ind]
		var src_name  = v["src"]
		var dest_name = v["dest"]
		var q = v["q"]
		
		var stri: String = "%s -> %s %d (%f, %f, %f, %f)\n" % [ src_name, dest_name, ind, q.w, q.x, q.y, q.z ]
		file.store_string( stri )
	
	file.close()


func _names_map():
	var names_map = {}
	var regex = RegEx.new()
	regex.compile("(?<src_name>\\S+)\\s*\\->\\s*(?<dest_name>\\S+)\\s*(?<ind>\\d+)\\s*\\((?<w>[\\-\\d\\.]+),\\s*(?<x>[\\-\\d\\.]+),\\s*(?<y>[\\-\\d\\.]+),\\s*(?<z>[\\-\\d\\.]+)\\s*\\)")
	var text: String = $Panel/Tabs/Capture/BoneCorrespondences.text
	for m in regex.search_all( text ):
		var src_name: String  = m.get_string( "src_name" )
		var dest_name: String = m.get_string( "dest_name" )
		var ind = int( m.get_string( "ind" ) )
		var w = float( m.get_string( "w" ) )
		var x = float( m.get_string( "x" ) )
		var y = float( m.get_string( "y" ) )
		var z = float( m.get_string( "z" ) )
		names_map[ind] = { "src": src_name, "dest": dest_name, "ind": ind, "q": Quat( x, y, z, w ) }
	
	return names_map




