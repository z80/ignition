extends Node

signal data_ready

var _db = null
var _frame_search = null

var _drag_rf = null

var _desc_weights: Array
var _desc_gains: Array
var _desc_lengths: Array

var _switch_threshold: float = 0.3
var _switch_period: int = 15

const HEAD_IND: int       = 0
const LEFT_HAND_IND: int  = 1
const RIGHT_HAND_IND: int = 2
const TORSO_IND: int      = 3
const LEFT_LEG_IND: int   = 4
const RIGHT_LEG_IND: int  = 5

const ROOT_IND: int       = TORSO_IND

const PARENTS: Array = [ 3, 3, 3, -1, 3, 3 ]

const FPS: float = 24.0
const DT: float  = 1.0/FPS
const POSE_LIMB_INDS: Array = [TORSO_IND, LEFT_LEG_IND, RIGHT_LEG_IND]
const TRAJ_TIME_MOMENTS: Array = [ 0.15, 0.30, 0.45, 0.60, 0.75, 1.00, 1.15 ]
var TRAJ_FRAME_INDS: Array = []



# Forward vector in local ref frame.
const V_HEADING_FWD: Vector3       = Vector3( 0.0, 0.0, -1.0 )



# Initial/default control.
# velocity relative to current azimuth.
var _control_input: Dictionary = {}
var _frame_ind: int = 0
var _switch_counter: int = 0
var _f: Array
var _control_pos_sequence: Array
var _control_vel_sequence: Array

# The state is defined by animation frame.
# And these two are used to place animation frame to 
# The right place in space.
# These two parameters are adjusted on every frame switch.
var _pose_q: Quat     = Quat.IDENTITY
var _pose_r: Vector3  = Vector3.ZERO
# Latest increments. Needed to adjust pose during switches.
var _pose_dq: Quat    = Quat.IDENTITY
var _pose_dr: Vector3 = Vector3.ZERO
# Time passed after last frame.
var _time_passed: float = 0.0


# user control input for visualization
var _vis_control_sequence: Array
var _print_control_sequence: Array


# Run the algorithm, or just demonstrate frames.
var _run_mm_algorithm:    bool = true
var _increment_frame_ind: bool = true


# Just after a jump implement smoothing.
const SMOOTHING_JUMP_FRAMES_QTY: int = 5
var _frames_after_jump_qty: int      = SMOOTHING_JUMP_FRAMES_QTY
var _frame_before_jump               = null
var _d_frame_before_jump             = null


# For debugging going to play back the same exact 
# database sequentially as if data went from VIVE.
var _dbg_frame_index: int = 0


# Called when the node enters the scene tree for the first time.
func _ready():
	# Remember! After assigning categoriesit is necessary to re-generate descriptors (!!!)
	# So the work pattern is 
	# 1) generate_descriptors( true, true )
	# 2) Assign categories.
	# 3) generate_descriptors( false, true )

	_init_frame_ind_array()
	
	#generate_descriptors( true, true )
	#generate_descriptors( false, true )
	init()


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process( delta ):
	# ******************************************************
	# This is for debugging only.
	# It is supposed to work with VIVE input instead.
	#var f = _db.get_frame( _dbg_frame_index )
	#if _dbg_frame_index < 128:
	#	_dbg_frame_index += 1
	#apply_controls( f )
	# ******************************************************
	
	process( delta )


func _input( event ):
	pass


# This one is called every time it starts.
# It assumes frames and descriptors present in the database.
func init():
	_drag_rf = $DraggedRf
	_drag_rf.reset()
	
	_db = _open_frame_database()
	
	_desc_weights = _db.get_config( "inv_std" )
	_desc_lengths = _db.get_config( "desc_lengths" )
	var th = _db.get_config( "switch_threshold" )
	if th == null:
		_switch_threshold = 0.3
		_db.set_config( "switch_threshold", _switch_threshold )
	else:
		_switch_threshold = th
	
	var dg = _db.get_config( "desc_gains" )
	if dg == null:
		dg  = [1.0, 1.0, 1.0, 1.0]
		_db.set_config( "desc_gains", dg )
	else:
		_desc_gains = dg
	
	_frame_search = _build_kd_tree()
	_apply_desc_gains()
	_init_control_sequence()
	
	_f = _frame_in_space( _frame_ind )
	
	emit_signal( "data_ready" )
	


func set_desc_gains( gains: Array ):
	var sz: int = _desc_gains.size()
	var needed_sz: int = _desc_gains.size()
	if ( sz != needed_sz ):
		gains.resize( needed_sz )
		for i in range( sz, needed_sz ):
			gains.push_back( 1.0 )
	_desc_gains = gains
	_db.set_config( "desc_gains", _desc_gains )
	_apply_desc_gains()


func get_desc_gains():
	return _desc_gains


func _apply_desc_gains():
	var ind: int = 0
	var qty: int = _desc_gains.size()
	var weights: Array = []
	for i in range( qty ):
		var len_i: int = _desc_lengths[i]
		var gain: float = _desc_gains[i]
		for j in range( len_i ):
			var weight: float = _desc_weights[ind]
			weight = weight * gain
			weights.push_back( weight )
			ind += 1
	_frame_search.set_weights( weights )


func set_switch_threshold( th: float ):
	_switch_threshold = th
	_db.set_config( "switch_threshold", _switch_threshold )


func get_switch_threshold():
	return _switch_threshold


func _build_kd_tree():
	print( "Reading frames database..." )
	
	var dims: int = _db.get_config( "desc_length" )
	var fs = FrameSearch.new()
	fs.set_dims( dims )
	
	var frames_qty: int = _db.frames_qty
	
	for i in range( frames_qty ):
		var d: Array = _db.get_desc( i )
		fs.append( d )
	
	print( "Building KdTree..." )
	fs.build_tree()
	print( "done" )
	
	return fs


# In order to make it immune to FPS specify actual 
# time in the past. And this thing is supposed to convert 
# to frame numbers.
func _init_frame_ind_array():
	TRAJ_FRAME_INDS = []
	for t in TRAJ_TIME_MOMENTS:
		var f_index: int = int( round( t * FPS ) )
		TRAJ_FRAME_INDS.push_back( f_index )



# Need to do this once
func generate_descriptors( fill_categories: bool = false, fill_descs: bool = true ):
	_db = _open_frame_database()
	if fill_categories:
		_fill_default_cats()
	if fill_descs:
		_fill_descs()
		_fill_desc_lengths()
		_estimate_scale()
	
	_db.close()


func _open_frame_database():
	var db = $FramesDb
	var res: bool = db.open()
	
	if not res:
		print( "ERROR: Failed to open frame database" )
		return null
		
	# Read bone names.
	#_bone_names = db.get_config( "names" )
	#for i in range(_bone_names.size()):
	#	print( "%d: %s" % [i, _bone_names[i]] )
	
	return db



func _fill_descs():
	# Number of frames
	var frames_qty: int = _db.frames_qty
	
	# Reset dragged ref. frame.
	_drag_rf = $DraggedRf
	_drag_rf.reset()
	
	var pose_desc: Array = []
	var traj_desc: Array = []
	var desc: Array = []
	
	for i in range( frames_qty ):
		#f = frame_( frame_db, i )
		var f = _db.get_frame( i )
		var torso_pose: Array = [ f[TORSO_IND], f[TORSO_IND+1], f[TORSO_IND+2], f[TORSO_IND+3], f[TORSO_IND+4], f[TORSO_IND+5], f[TORSO_IND+6] ]
		_drag_rf.process( torso_pose )
		var rf_r: Vector3 = _drag_rf.position()
		var rf_q: Quat    = _drag_rf.rotation()
		_db.set_rf( i, rf_q, rf_r )
		
		pose_desc = _pose_desc( i )
		traj_desc = _traj_desc( i )
		desc = []
		for d in pose_desc:
			for v in d:
				desc.push_back( v )
		for d in traj_desc:
			for v in d:
				desc.push_back( v )
		
		_db.set_desc( i, desc )
		
	return true


func _fill_desc_lengths():
	# Write figure out descriptor lengths
	#var f = frame_( db, 0 )
	var pose_desc = _pose_desc( 0 )
	var traj_desc = _traj_desc( 0 )
	var descs: Array = pose_desc + traj_desc
	var desc: Array = []
	var desc_lengths: Array = []
	for d in descs:
		var sz = d.size()
		desc_lengths.push_back( sz )
		desc += d
	var desc_length: int = desc.size()
	
	_db.set_config( "desc_length",  desc_length )
	_db.set_config( "desc_lengths", desc_lengths )
	
	var gains: Array = []
	for i in desc_lengths:
		gains.push_back( 1.0 )
	
	_db.set_config( "desc_gains", gains )
	_db.set_config( "switch_threshold", _switch_threshold )


func _fill_default_cats():
	# Number of frames
	#db.query("SELECT COUNT(*) AS 'qty' FROM " + TABLE_DATA_NAME + ";")
	#var frames_qty: int = db.query_result[0]['qty']
	
	var cat = [0]
	var res: bool = _db.assign_default_category( cat )
	if not res:
		print( "Failed to write default value into category " )
		return false
	
	return true


func assign_cat( from: int, to: int, category: int ):
	var res: bool = _db.assign_category( from, to, [category] )
	if not res:
		print( "Failed to write value into category column" )
		return false
	
	return true





func _frame_in_space( index: int ):
	var f = _db.get_frame( index )
	var sz: int = f.size()
	var f_dest: Array = []
	f_dest.resize( sz )
	
	var rf = _db.get_rf( index )
	var q_rf: Quat    = rf.q
	var r_rf: Vector3 = rf.r
	var inv_r_rf: Quat = q_rf.inverse()
	
	#var base_q_root = _quat_azimuth( q_root )
	#var inv_base_q_root = base_q_root.inverse()
	#var q_adj = _pose_q * inv_base_q_root
	
	var r_at = _drag_rf.position()
	var q_at = _drag_rf.rotation()
	
	var ind: int = 0
	for i in range( 0, sz, 7 ):
		var q: Quat = Quat( f[i+1], f[i+2], f[i+3], f[i] )
		var r: Vector3 = Vector3( f[i+4], f[i+5], f[i+6] )
		q = q_at * inv_r_rf * q
		r = r - r_rf
		r = q_at.xform( inv_r_rf.xform( r ) )
		r += r_at
		
		f_dest[i]   = q.w
		f_dest[i+1] = q.x
		f_dest[i+2] = q.y
		f_dest[i+3] = q.z
		f_dest[i+4] = r.x
		f_dest[i+5] = r.y
		f_dest[i+6] = r.z
		
		ind += 1
	
	return f_dest


func _frame_in_space_smoothed( index: int ):
	var f_next = _db.get_frame( index )
	var sz: int = f_next.size()
	
	var t: float = float(_frames_after_jump_qty) / float(SMOOTHING_JUMP_FRAMES_QTY)

	var rf = _db.get_rf( index )
	var q_root_next: Quat    = rf.q
	var r_root_next: Vector3 = rf.r
	var az_inv_q_root_next = _quat_azimuth( q_root_next ).inverse()
	
	var f_prev = _frame_before_jump
	
	var f_dest: Array = []
	f_dest.resize( sz )
	
	var pose_r: Vector3 = _drag_rf.position()
	var pose_q: Quat    = _drag_rf.rotation()
	
	for i in range(0, sz, 7):
		var q0: Quat = Quat( f_prev[i+1], f_prev[i+2], f_prev[i+3], f_prev[i] )
		var q1: Quat = Quat( f_next[i+1], f_next[i+2], f_next[i+3], f_next[i] )
		var r0: Vector3 = Vector3( f_prev[i+4], f_prev[i+5], f_prev[i+6] )
		var r1: Vector3 = Vector3( f_next[i+4], f_next[i+5], f_next[i+6] )
		
		q1 = az_inv_q_root_next * q1
		r1 = r1 - r_root_next
		r1 = az_inv_q_root_next.xform( r1 )
		
		var q: Quat    = _slerp_quat( q0, q1, t )
		var r: Vector3 = _slerp_vector3( r0, r1, t )
		q = pose_q * q
		r = pose_q.xform( r )
		r = r + pose_r
		f_dest[i]   = q.w
		f_dest[i+1] = q.x
		f_dest[i+2] = q.y
		f_dest[i+3] = q.z
		f_dest[i+4] = r.x
		f_dest[i+5] = r.y
		f_dest[i+6] = r.z
	
	return f_dest













func _estimate_scale():
	print( "Estimating scale..." )
	var dims: int = _db.get_config( "desc_length" )
	
	var frames_qty: int = _db.frames_qty

	var fs = FrameSearch.new()
	fs.set_dims( dims )
	
	for i in range( frames_qty ):
		var d: Array = _db.get_desc( i )
		fs.append( d )
	
	var inv_std: Array  = fs.inv_std()
	var inv_ampl: Array = fs.inv_ampl()
	
	_db.set_config( "inv_std", inv_std )
	_db.set_config( "inv_ampl", inv_ampl )


func _pose_desc( index: int ):
	var f = _db.get_frame( index )
	var fp
	if ( index > 0 ):
		fp = _db.get_frame( index-1 )
	else:
		fp = f
	
	# Root pose for drag rf.
	# Here take current drag rf - not recorded one as this happens when 
	# descriptors are generated and saved.
	var root_r: Vector3 = _drag_rf.position()
	var az_root_q_inv: Quat = _drag_rf.rotation().inverse()
	
	var desc_r: Array
	var desc_v: Array
	#var desc_c: Array
	for limb_ind in POSE_LIMB_INDS:
		var ind: int = limb_ind * 7
		var q: Quat = Quat( f[ind+1], f[ind+2], f[ind+3], f[ind+0] )
		var r: Vector3 = Vector3( f[ind+4], f[ind+5], f[ind+6] )
		#var qp: Quat = Quat( fp[ind+1], fp[ind+2], fp[ind+3], fp[ind+0] )
		var rp: Vector3 = Vector3( fp[ind+4], fp[ind+5], fp[ind+6] )
		r  -= root_r
		rp -= root_r
		r  = az_root_q_inv.xform( r )
		rp = az_root_q_inv.xform( rp )
		
		var v: Vector3 = (r - rp) * FPS
		desc_r += [ r.x, r.y, r.z ]
		desc_v += [ v.x, v.y, v.z ]
	
	#desc_c = _db.get_category( index )
	
	return [ desc_r, desc_v ]


func _traj_desc( index: int ):
	var f = _db.get_frame( index )
	
	# Root pose
	var root_r: Vector3 = _drag_rf.position()
	var az_root_q_inv: Quat = _drag_rf.rotation().inverse()
	
	var desc_r: Array = []
	var desc_heading: Array = []
	#var desc_c: Array = []
	
	var max_ind = _db.frames_qty - 1
	for frame_delta_ind in TRAJ_FRAME_INDS:
		var frame_ind: int = index + frame_delta_ind
		if frame_ind < 0:
			frame_ind = 0
		elif frame_ind > max_ind:
			frame_ind = max_ind
		
		f = _db.get_frame( frame_ind )
		
		var ind: int = TORSO_IND * 7
		var q: Quat = Quat( f[ind+1], f[ind+2], f[ind+3], f[ind+0] )
		#q = _quat_azimuth( q )
		q = az_root_q_inv * q
		var v_heading: Vector3 = q.xform( V_HEADING_FWD )
		desc_heading += [ v_heading.x, v_heading.y, v_heading.z ]
		
		var r: Vector3 = Vector3( f[ind+4], f[ind+5], f[ind+6] )
		r  -= root_r
		r  = az_root_q_inv.xform( r )
		
		desc_r += [ r.x, r.y, r.z ]
		
		#desc_c += _db.get_category( frame_ind )
	
	return [ desc_r, desc_heading ]


func _input_based_pose_desc( cat: Array = [0] ):
	var f = _db.get_frame( _frame_ind )
	var fp
	if ( _frame_ind > 0 ):
		fp = _db.get_frame( _frame_ind-1 )
	else:
		fp = f
	
	# Root pose for drag rf.
	# Here take current drag rf - not recorded one as this happens when 
	# descriptors are generated and saved.
	var root_r: Vector3 = _drag_rf.position()
	var az_root_q_inv: Quat = _drag_rf.rotation().inverse()
	
	var desc_r: Array
	var desc_v: Array
	#var desc_c: Array
	for limb_ind in POSE_LIMB_INDS:
		var ind: int = limb_ind * 7
		var q: Quat = Quat( f[ind+1], f[ind+2], f[ind+3], f[ind+0] )
		var r: Vector3 = Vector3( f[ind+4], f[ind+5], f[ind+6] )
		#var qp: Quat = Quat( fp[ind+1], fp[ind+2], fp[ind+3], fp[ind+0] )
		var rp: Vector3 = Vector3( fp[ind+4], fp[ind+5], fp[ind+6] )
		r  -= root_r
		rp -= root_r
		r  = az_root_q_inv.xform( r )
		rp = az_root_q_inv.xform( rp )
		
		var v: Vector3 = (r - rp) * FPS
		desc_r += [ r.x, r.y, r.z ]
		desc_v += [ v.x, v.y, v.z ]
	
	#desc_c = _db.get_category( index )
	
	return [ desc_r, desc_v ]


func _input_based_traj_desc( cat: Array = [0] ):
	var qty: int = _control_pos_sequence.size()
	var index: int = qty-1
	
	# Root pose
	var az_drag_q_inv: Quat = _drag_rf.rotation().inverse()
	
	var desc_r: Array = []
	var desc_heading: Array = []
	#var desc_c: Array = []
	
	for frame_delta_ind in TRAJ_FRAME_INDS:
		var f = _control_pos_sequence[ frame_delta_ind ]
		var r = f[0]
		var h = f[1]
		r = az_drag_q_inv.xform( r )
		h = az_drag_q_inv.xform( h )
		
		desc_heading += [ h.x, h.y, h.z ]
		desc_r += [ r.x, r.y, r.z ]
			
		#desc_c += cat
	
	return [ desc_r, desc_heading ]


func _frame_azimuth( ind: int ):
	var f = _db.get_frame( ind )
	var root_ind: int = ROOT_IND*7
	var q: Quat = Quat( f[root_ind+1], f[root_ind+2], f[root_ind+3], f[root_ind] )
	var az_q: Quat = _quat_azimuth( q )
	return az_q


func _quat_azimuth( q: Quat ):
	var fwd: Vector3 = Vector3.RIGHT
	var q_norm: Quat = q.normalized()
	#print( "quat_qzimuth_(", q, ")" )
	fwd = q_norm.xform( fwd )
	var ang2: float = 0.5 * atan2( -fwd.z, fwd.x )
	var co2: float = cos( ang2 )
	var si2: float = sin( ang2 )
	var res_q: Quat = Quat( 0.0, si2, 0.0, co2 )
	return res_q


func _init_control_sequence():
	_control_input = {
		tps = true, 
		v = Vector3.ZERO, 
		q = Quat.IDENTITY
	}
	
	var sz = TRAJ_FRAME_INDS.size()
	var qty = TRAJ_FRAME_INDS[sz-1]
	if qty < 0:
		qty = -qty
	qty += 1
	
	_control_vel_sequence = []
	for i in range(qty):
		var vv = [ Vector3.ZERO, V_HEADING_FWD ]
		_control_vel_sequence.push_back( vv )




func apply_controls( ctrl_v: Vector3, cam_t: Transform = Transform(), tps: bool = true ):
	var q: Quat = cam_t.basis
	q = _quat_azimuth( q )
	_control_input = {
		tps    = tps, 
		v = ctrl_v, 
		az_cam_q  = q
	}


func _updata_control_sequence( cat: int = 0 ):
	# Here we assume not so much time passed since 
	# It was aupdated. It is essential as body orientation is used 
	# in order to compute control relative in drag frame.
	var ctrl = _control_input.duplicate()
	
	var cam_q: Quat  = ctrl.az_cam_q
	var drag_q: Quat = _drag_rf.rotation()
	
	# Compute velocity and heading in global rf. Yes, not in local rf.
	# This is in order to be able to compute descriptor on purpose.
	var v: Vector3 = cam_q.xform( ctrl.v )
	# Heading computed depending on if it is first or third person case.
	var heading: Vector3
	if ctrl.tps:
		heading = cam_q.xform( V_HEADING_FWD )
	else:
		# Here need to compute torso heading.
		var q: Quat = drag_q * _pose_q
		heading = q.xform( V_HEADING_FWD )
	_control_vel_sequence.push_back( [v, heading] )
	
	var sz = TRAJ_FRAME_INDS.size()
	var qty = TRAJ_FRAME_INDS[sz-1]
	if qty < 0:
		qty = -qty
	qty += 1
	
	while _control_vel_sequence.size() > qty:
		_control_vel_sequence.pop_front()
	
	# Integrate velocities and convert all to local ref. frame.
	_control_pos_sequence.resize( qty )
	var r: Vector3  = Vector3.ZERO
	
	# First step integrate.
	for i in range( qty ):
		var vv = _control_vel_sequence[i]
		var dr: Vector3 = vv[0]
		var h:  Vector3 = vv[1]
		dr *= DT
		r += dr
		var pp = [r, h]
		_control_pos_sequence[i] = pp






func _update_vis_control_sequence():
	return
	
	var qty: int = _control_pos_sequence.size()
	_vis_control_sequence.resize( qty )
	
	#var f = frame_( db_, frame_ind_ )
	#var q: Quat    = Quat( f[ROOT_IND+1], f[ROOT_IND+2], f[ROOT_IND+3], f[ROOT_IND] )
	#var r: Vector3 = Vector3( f[ROOT_IND+4], f[ROOT_IND+5], f[ROOT_IND+6] )
	#var az_q: Quat = quat_azimuth_( q )
	#var inv_az_q = az_q.inverse()
	
	for i in range( qty ):
		var c2: Vector2 = _control_pos_sequence[i]
		var c3: Vector3 = Vector3(c2.x, c2.y, 1.0 )
		#c3 = pose_q_.xform( c3 )
		c3 = c3 + _pose_r
		_vis_control_sequence[i] = c3
	
	
	var ind: int = 0
	qty = TRAJ_FRAME_INDS.size()
	_print_control_sequence.resize( qty )
	var inv_pose_q: Quat = _pose_q.inverse()
	var q_frame_az: Quat = _frame_azimuth( _frame_ind )
	var q: Quat = q_frame_az * inv_pose_q
	for i in TRAJ_FRAME_INDS:
		var v = _control_pos_sequence[i-1]
		var v3 = Vector3( v.x, v.y, 0.0 )
		v3 = q.xform( v3 )
		v = Vector2( v3.x, v3.y )
		_print_control_sequence[ind] = v
		ind += 1


func _update_dragged_ref_frame():
	_drag_rf.process( _pose_q, _pose_r )

# This one goes into _process( delta ).
# It calls process_frame() when needed.
func process( delta ):
	_time_passed += delta
	while ( _time_passed >= DT ):
		process_frame()
		_time_passed -= DT


# Assuming time passed is exactly one frame.
func process_frame():
	var time_to_switch: bool
	
	if _run_mm_algorithm:
		if _switch_counter < _switch_period:
			time_to_switch = false
		else:
			time_to_switch = true
			_switch_counter -= _switch_period
		
		# Increment frame switch counter.
		_switch_counter += 1
	else:
		time_to_switch = false
	
	
	var next_ind: int = _frame_ind
	if _increment_frame_ind:
		next_ind += 1
		if ( next_ind >= _db.frames_qty ):
			next_ind = _db.frames_qty - 1
	
	
	# For debugging
	#time_to_switch = false
	
	var jump: bool = false
	
	# Update control sequence based on most recent user input.
	_updata_control_sequence()
	# For control sequence visualization.
	#_update_vis_control_sequence()
	
	# Update dregged ref. frame state.
	_update_dragged_ref_frame()
	
	if time_to_switch:
		#var f_cur = frame_( db_, frame_ind_ )
		var desc_p = _input_based_pose_desc()
		var desc_t = _input_based_traj_desc()
		print( "traj desc: ", desc_t )
		var d = []
		for di in desc_p:
			for v in di:
				d.push_back( v )
		for di in desc_t:
			for v in di:
				d.push_back( v )
		var closest_err: float = _frame_search.nearest( d )
		var closest_ind: int   = _frame_search.nearest_ind()
		if closest_ind != next_ind:
			var next_err: float = _frame_search.dist( d, next_ind )
			var improvement: float = next_err - closest_err
			if improvement >= _switch_threshold:
				next_ind = closest_ind
				jump = true
				print( "jump from ", _frame_ind, " to ", next_ind )
			
	var fp = _db.get_frame( _frame_ind )
	var fn = _db.get_frame( next_ind )
	
	var root_ind: int = ROOT_IND*7
	var qp: Quat    = Quat( fp[root_ind+1], fp[root_ind+2], fp[root_ind+3], fp[root_ind] )
	var rp: Vector3 = Vector3( fp[root_ind+4], fp[root_ind+5], fp[root_ind+6] )
	var qn: Quat    = Quat( fn[root_ind+1], fn[root_ind+2], fn[root_ind+3], fn[root_ind] )
	var rn: Vector3 = Vector3( fn[root_ind+4], fn[root_ind+5], fn[root_ind+6] )
	
	#print( "a" )
	var az_qp: Quat = _quat_azimuth( qp )
	#print( "cc" )
	var az_qn: Quat = _quat_azimuth( qn )
	#print( "b" )
	
	if jump:
		# Also remember current frame and reset smoothing frames qty.
		#frame_before_jump_ = _copy_array( fp )
		var v = _prepare_frame_and_d_frame( _frame_ind )
		_frame_before_jump = v[0]
		_d_frame_before_jump = v[1]
		_frames_after_jump_qty = 0
		#print( "fp size:                 ", fp.size() )
		#print( "frame_before_jump_ size: ", frame_before_jump_.size() )
	
	if _frames_after_jump_qty < SMOOTHING_JUMP_FRAMES_QTY:
		_frames_after_jump_qty += 1
	
	var dq: Quat
	var dr: Vector3
	if jump:
		dr = _pose_dr
		dq = _pose_dq
	
	else:
		var inv_az_qp: Quat = az_qp.inverse()
		dq = inv_az_qp * az_qn
		dr = rn - rp
		dr = inv_az_qp.xform( dr )
		dr = _pose_q.xform( dr )
		_pose_dr = dr
		_pose_dq = dq
	
	#var rz: float = pose_r_.z
	_pose_r += dr
	
	# To make it not go up or down.
	if jump:
		_pose_r.y = 0.0
	#pose_r_.z = rz
	
	#if abs(dq.z) > 0.015:
	#	print( "Suspicious activity detected at frame # ", frame_ind_ )
	#	print( "fp: ", fp )
	#	print( "fn: ", fn )
	_pose_q = _pose_q * dq
	_pose_q = _quat_azimuth( _pose_q )
	
	#print( "pose q: ", _pose_q )
	
	#print( "frame #%d q:(%f, %f, %f, %f), dq:(%f, %f, %f, %f), pq:(%f, %f, %f, %f)" %[ frame_ind_, qn.w, qn.x, qn.y, qn.z, dq.w, dq.x, dq.y, dq.z, pose_q_.w, pose_q_.x, pose_q_.y, pose_q_.z ] )
	

	
	_frame_ind = next_ind
	if (_frame_before_jump == null) or (_frames_after_jump_qty >= SMOOTHING_JUMP_FRAMES_QTY):
		_f = _frame_in_space( _frame_ind )
	else:
		_increment_frame()
		_f = _frame_in_space_smoothed( _frame_ind )
	
	# For debug purposes temporarily no temporal smoothing.
	#_f = _frame_in_space( _frame_ind )


func frame():
	var f: Array = []
	for i in range(6):
		var ind: int = i * 7
		var link = {
			q = Quat( _f[ind+1], _f[ind+2], _f[ind+3], _f[ind] ), 
			r = Vector3( _f[ind+4], _f[ind+5], _f[ind+6] )
		}
		f.push_back( link )
	return f




func _on_tree_exiting():
	if ( is_instance_valid( _db ) ):
		_db.close()
		_db = null




func _slerp_frame( f0: Array, f1: Array, t: float ):
	var qty: int = f0.size()
	var f = []
	f.resize( qty )
	for i in range(0, qty, 7):
		var q0: Quat = Quat( f0[i+1], f0[i+2], f0[i+3], f0[i] )
		var q1: Quat = Quat( f1[i+1], f1[i+2], f1[i+3], f1[i] )
		var r0: Vector3 = Vector3( f0[i+4], f0[i+5], f0[i+6] )
		var r1: Vector3 = Vector3( f1[i+4], f1[i+5], f1[i+6] )
		var q: Quat    = _slerp_quat( q0, q1, t )
		var r: Vector3 = _slerp_vector3( r0, r1, t )
		f[i]   = q.w
		f[i+1] = q.x
		f[i+2] = q.y
		f[i+3] = q.z
		f[i+4] = r.x
		f[i+5] = r.y
		f[i+6] = r.z
	
	return f


func _slerp_quat( q0: Quat, q1: Quat, t: float ):
	var dot: float = q0.w*q1.w + q0.x*q1.x + q0.y*q1.y + q0.z*q1.z;
	if dot < 0.0:
		q1 = -q1
		dot = -dot;
	
	# If almost the same quats interpolate linearly and 
	# normalize.
	if dot > 0.9995:
		var q: Quat = q0*(1.0-t) + q1*t
		q = q.normalized()
		return q
	
	var theta_0: float     = acos(dot)
	var theta: float       = theta_0 * t
	var sin_theta: float   = sin(theta)
	var sin_theta_0: float = sin(theta_0)
	
	var s0: float = cos(theta) - dot * sin_theta / sin_theta_0;
	var s1: float = sin_theta / sin_theta_0;
	var res: Quat = (q0 * s0) + (q1 * s1)
	return res


func _slerp_vector3( r0: Vector3, r1: Vector3, t: float ):
	var r: Vector3 = r0*(1.0-t) + r1*t
	return r



func _prepare_frame_and_d_frame( ind: int ):
	var ind1: int = ind
	var ind0: int
	if ( ind > 0 ):
		ind0 = ind - 1
	else:
		ind0 = ind
	
	var f0 = _db.get_frame( ind0 )
	var f1 = _db.get_frame( ind1 )
	
	var sz0: int = f0.size()
	var sz1: int = f1.size()
	
	var sz: int
	if sz0 <= sz1:
		sz = sz0
	else:
		sz = sz1
	
	var rf = _db.get_rf( ind1 )
	var q_root: Quat = rf["q"]
	var r_root: Vector3 = rf["r"]
	var az_inv_q_root = _quat_azimuth( q_root ).inverse()
	
	
	var f_dest: Array = []
	f_dest.resize( sz )
	var d_f_dest: Array = []
	d_f_dest.resize( sz )
	
	for i in range(0, sz, 7):
		var q0: Quat = Quat( f0[i+1], f0[i+2], f0[i+3], f0[i] )
		var q1: Quat = Quat( f1[i+1], f1[i+2], f1[i+3], f1[i] )
		var r0: Vector3 = Vector3( f0[i+4], f0[i+5], f0[i+6] )
		var r1: Vector3 = Vector3( f1[i+4], f1[i+5], f1[i+6] )
		q0 = az_inv_q_root * q0
		r0 = r0 - r_root
		r0 = az_inv_q_root.xform( r0 )
		
		q1 = az_inv_q_root * q1
		r1 = r1 - r_root
		r1 = az_inv_q_root.xform( r1 )
		
		f_dest[i]   = q1.w
		f_dest[i+1] = q1.x
		f_dest[i+2] = q1.y
		f_dest[i+3] = q1.z
		f_dest[i+4] = r1.x
		f_dest[i+5] = r1.y
		f_dest[i+6] = r1.z
		
		r0 = r1 - r0
		q0 = q0.inverse() * q1
		d_f_dest[i]   = q0.w
		d_f_dest[i+1] = q0.x
		d_f_dest[i+2] = q0.y
		d_f_dest[i+3] = q0.z
		d_f_dest[i+4] = r0.x
		d_f_dest[i+5] = r0.y
		d_f_dest[i+6] = r0.z
		
	return [f_dest, d_f_dest]


func _increment_frame():
	var f  = _frame_before_jump
	var df = _d_frame_before_jump
	var sz: int = df.size()
	for i in range(0, sz, 7):
		var q:  Quat = Quat( f[i+1], f[i+2], f[i+3], f[i] )
		var dq: Quat = Quat( df[i+1], df[i+2], df[i+3], df[i] )
		var r:  Vector3 = Vector3( f[i+4], f[i+5], f[i+6] )
		var dr: Vector3 = Vector3( df[i+4], df[i+5], df[i+6] )
		
		r += dr
		q = q * dq
		q = q.normalized()
		_frame_before_jump[i]   = q.w
		_frame_before_jump[i+1] = q.x
		_frame_before_jump[i+2] = q.y
		_frame_before_jump[i+3] = q.z
		_frame_before_jump[i+4] = r.x
		_frame_before_jump[i+5] = r.y
		_frame_before_jump[i+6] = r.z



