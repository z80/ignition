extends Node

signal data_ready

const SQLite = preload("res://addons/godot-sqlite/bin/gdsqlite.gdns")
const FRAME_DB_NAME = "res://mm_data.sqlite3.db"
var db_ = null
const TABLE_DATA_NAME   = "data"
const TABLE_CONFIG_NAME = "config"
var frames_qty_: int = -1
var frame_search_ = null

var bone_names_ = null

var desc_weights_: Array
var desc_gains_: Array
var desc_lengths_: Array

var switch_threshold_: float = 0.3
var switch_period_: int = 5

const ROOT_IND: int       = 0
const LEFT_LEG_IND: int   = 15
const RIGHT_LEG_IND: int  = 19
const LEFT_HAND_IND: int  = 8
const RIGHT_HAND_IND: int = 12
#const POSE_LIMB_INDS: Array = [ROOT_IND, LEFT_LEG_IND, RIGHT_LEG_IND, LEFT_HAND_IND, RIGHT_HAND_IND]
#const TRAJ_FRAME_INDS: Array = [30, 60, 90, 120]
const POSE_LIMB_INDS: Array = [ROOT_IND, LEFT_LEG_IND, RIGHT_LEG_IND]
const TRAJ_FRAME_INDS: Array = [15, 30, 45, 60]

const FPS: float = 60.0
const DT: float  = 1.0/FPS


# Movement constants for building future trajectory.
var slow_speed_: float = 0.5
var walk_speed_: float = 1.0
var fast_speed_: float  = 2.0

# Initial/default control.
# velocity relative to current azimuth.
var control_input_: Vector2 = Vector2(0.0, 0.0)
var frame_ind_: int = 0
var switch_counter_: int = 0
var f_: Array
var control_pos_sequence_: Array
var control_vel_sequence_: Array

# The state is defined by animation frame.
# And these two are used to place animation frame to 
# The right place in space.
# These two parameters are adjusted on every frame switch.
var pose_q_: Quat     = Quat.IDENTITY
var pose_r_: Vector3  = Vector3.ZERO
# Latest increments. Needed to adjust pose during switches.
var pose_dq_: Quat    = Quat.IDENTITY
var pose_dr_: Vector3 = Vector3.ZERO
# Time passed after last frame.
var time_passed_: float = 0.0

# Target skeleton
var skeleton_: Skeleton = null

# Parent index for each bone
var parents_: Array


# user control input for visualization
var vis_control_sequence_: Array
var print_control_sequence_: Array
var cam_q_: Quat
var cam_rel_q_: Quat

# Called when the node enters the scene tree for the first time.
func _ready():
	generate_descriptors()
	#init()


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process( delta ):
	pass
	
	#var t: Transform = Transform.IDENTITY
	#var w: bool = Input.is_action_pressed( "walk_w" )
	#var s: bool = Input.is_action_pressed( "walk_s" )
	#var a: bool = Input.is_action_pressed( "walk_a" )
	#var d: bool = Input.is_action_pressed( "walk_d" )
	#var fast: bool = Input.is_action_pressed( "walk_fast" )
	#var slow: bool = Input.is_action_pressed( "walk_slow" )
	#generate_controls( t, w, s, a, d, fast, slow )
	
	#time_passed_ += delta
	#while ( time_passed_ >= DT ):
	#	process_frame()
	#	time_passed_ -= DT


func _input( event ):
	pass

# This one is called every time it starts.
# It assumes frames and descriptors present in the database.
func init():
	var db = open_frame_database()
	db_ = db

	bone_names_   = get_config_( db, "names" )
	desc_weights_ = get_config_( db, "inv_std" )
	desc_lengths_ = get_config_( db, "desc_lengths" )
	var th = get_config_( db, "switch_threshold" )
	if th == null:
		switch_threshold_ = 0.3
		set_config_( db, "switch_threshold", switch_threshold_ )
	else:
		switch_threshold_ = th
	
	var sp = get_config_( db, "switch_period" )
	if sp == null:
		switch_period_ = 30
		set_config_( db, "switch_period", switch_period_ )
	else:
		switch_period_ = sp
	
	var dg = get_config_( db, "desc_gains" )
	if dg == null:
		dg  = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0]
		set_config_( db_, "desc_gains", dg )
	else:
		desc_gains_ = dg
	
	frame_search_ = build_kd_tree_( db )
	apply_desc_gains_()
	init_control_sequence_()
	
	f_ = frame_in_space_( db_, frame_ind_ )
	
	get_parent_skeleton()
	
	var ps = get_config_( db_, "parents" )
	if ps == null:
		parents_ = get_parents_array_()
	else:
		parents_ = ps
	
	emit_signal( "data_ready" )
	


func set_desc_gains( gains: Array ):
	var sz: int = desc_gains_.size()
	var needed_sz: int = desc_gains_.size()
	if ( sz != needed_sz ):
		gains.resize( needed_sz )
		for i in range( sz, needed_sz ):
			gains.push_back( 1.0 )
	desc_gains_ = gains
	set_config_( db_, "desc_gains", desc_gains_ )
	apply_desc_gains_()


func get_desc_gains():
	return desc_gains_


func apply_desc_gains_():
	var ind: int = 0
	var qty: int = desc_gains_.size()
	var weights: Array = []
	for i in range( qty ):
		var len_i: int = desc_lengths_[i]
		var gain: float = desc_gains_[i]
		for j in range( len_i ):
			var weight: float = desc_weights_[ind]
			weight = weight * gain
			weights.push_back( weight )
			ind += 1
	frame_search_.set_weights( weights )


func set_switch_threshold( th: float ):
	switch_threshold_ = th
	set_config_( db_, "switch_threshold", switch_threshold_ )


func get_switch_threshold():
	return switch_threshold_


func build_kd_tree_( db ):
	print( "Reading frames database..." )
	
	var dims: int = get_config_( db, "desc_length" )
	var fs = FrameSearch.new()
	fs.set_dims( dims )
	
	db.query("SELECT COUNT(*) AS 'qty' FROM " + TABLE_DATA_NAME + ";")
	var frames_qty: int = db.query_result[0]['qty']
	
	for i in range( frames_qty ):
		var d: Array = desc_( db, i )
		fs.append( d )
	
	print( "Building KdTree..." )
	fs.build_tree()
	print( "done" )
	
	return fs






# Need to do this once
func generate_descriptors():
	var db = open_frame_database()
	create_desc_column( db )
	fill_descs( db )
	estimate_scale( db )
	db.close_db()


func open_frame_database():
	var db = SQLite.new()
	db.path = FRAME_DB_NAME
	var res: bool = db.open_db()
	
	if not res:
		print( "Failed to open frame database ", FRAME_DB_NAME )
		return null
		
	db.query("SELECT COUNT(*) AS 'qty' FROM " + TABLE_DATA_NAME + ";")
	frames_qty_ = db.query_result[0]['qty']
	
	# Read bone names.
	bone_names_ = get_config_( db, "names" )
	
	return db


func has_desc_column( db ):
	var cmd: String = "SELECT COUNT(*) AS qty FROM pragma_table_info('desc') WHERE name='data';"
	var res: bool = db.query( cmd )
	var qty: int = db.query_result[0]["qty"]
	var has_desc: bool = (qty > 0)
	#print( "query result: ", has_desc )
	return has_desc


func create_desc_column( db ):
	var cmd: String = "ALTER TABLE data ADD COLUMN desc text;"
	var res: bool = db.query( cmd )
	if not res:
		print( "failed to add column \'desc\' to the table \'data\'" )
		return false
	return true


func fill_descs( frame_db ):
	# Number of frames
	frame_db.query("SELECT COUNT(*) AS 'qty' FROM " + TABLE_DATA_NAME + ";")
	var frames_qty: int = frame_db.query_result[0]['qty']
	
	# First figure out descriptor lengths
	var f = frame_( frame_db, 0 )
	var pose_desc = pose_desc_( frame_db, 0 )
	var traj_desc = traj_desc_( frame_db, 0 )
	var descs: Array = pose_desc + traj_desc
	var desc: Array = []
	var desc_lengths: Array = []
	for d in descs:
		var sz = d.size()
		desc_lengths.push_back( sz )
		desc += d
	var desc_length: int = desc.size()
	
	set_config_( frame_db, "desc_length",  desc_length )
	set_config_( frame_db, "desc_lengths", desc_lengths )
	
	for i in range( frames_qty ):
		f = frame_( frame_db, i )
		pose_desc = pose_desc_( frame_db, i )
		traj_desc = traj_desc_( frame_db, i )
		descs = pose_desc + traj_desc
		desc = []
		for d in descs:
			desc += d
		
		var stri_f: String = JSON.print( f )
		stri_f = stri_f.replace( "\"", "\'" )
		var stri_d: String = JSON.print( desc )
		stri_d = stri_d.replace( "\"", "\'" )
		var cmd = "INSERT OR REPLACE INTO data(id, data, desc) VALUES(%d, \'%s\', \'%s\');" % [i, stri_f, stri_d]
		var res: bool = frame_db.query( cmd )
		if not res:
			print( "Failed to write into desc_db" )
			return false
		
	return true


func set_config_( db, key: String, data ):
	var stri: String = JSON.print( data )
	stri = stri.replace( "\"", "\'" )
	var cmd: String = "INSERT OR REPLACE INTO config(id, data) VALUES (\'%s\', \'%s\');" % [key, stri]
	var res: bool = db.query( cmd )
	return res


func get_config_( db, key: String ):
	var cmd: String = "SELECT data FROM config WHERE id=\'%s\' LIMIT 1;" % key
	#var cmd: String = "SELECT FROM config(id, data);"
	var res: bool = db.query( cmd )
	if not res:
		print( "config query failed" )
		return null
	var q_res: Array = db.query_result
	if q_res.size() < 1:
		return null
	var stri: String = q_res[0]['data']
	stri = stri.replace( "\'", "\"" )
	var ret = JSON.parse( stri )
	ret = ret.result
	return ret



func frame_( db, index: int ):
	var cmd: String = "SELECT data FROM data WHERE id = %d LIMIT 1;" % index
	var res: bool = db.query( cmd )
	if not res:
		print( "failed to query frame from the db" )
		return null
	var selected_array : Array = db.query_result
	var stri = selected_array[0]['data']
	stri = stri.replace( "'", "\"" )
	var ret = JSON.parse( stri )
	ret = ret.result
	return ret


func frame_in_space_( db, index: int ):
	var f = frame_( db, index )
	var sz: int = f.size()
	var f_dest: Array
	f_dest.resize( sz )
	
	var q_root: Quat = Quat( f[ROOT_IND+1], f[ROOT_IND+2], f[ROOT_IND+3], f[ROOT_IND] )
	var r_root: Vector3 = Vector3( f[ROOT_IND+4], f[ROOT_IND+5], f[ROOT_IND+6] )
	
	var base_q_root = quat_azimuth_( q_root )
	var inv_base_q_root = base_q_root.inverse()
	var q_adj = pose_q_ * inv_base_q_root
	
	var inv_q_root = q_root.inverse()
	var dest_q_root = q_adj * q_root
	var dest_r_root = pose_r_
	f_dest[ROOT_IND]   = dest_q_root.w
	f_dest[ROOT_IND+1] = dest_q_root.x
	f_dest[ROOT_IND+2] = dest_q_root.y
	f_dest[ROOT_IND+3] = dest_q_root.z
	f_dest[ROOT_IND+4] = dest_r_root.x
	f_dest[ROOT_IND+5] = dest_r_root.y
	f_dest[ROOT_IND+6] = dest_r_root.z
	
	var ind: int = 0
	for i in range( 0, sz, 7 ):
		if ( ind == ROOT_IND  ):
			ind += 1
			continue
		var q: Quat = Quat( f[i+1], f[i+2], f[i+3], f[i] )
		var r: Vector3 = Vector3( f[i+4], f[i+5], f[i+6] )
		q = q_adj * q
		r = r - r_root
		r = q_adj.xform( r )
		r += pose_r_
		
		f_dest[i]   = q.w
		f_dest[i+1] = q.x
		f_dest[i+2] = q.y
		f_dest[i+3] = q.z
		f_dest[i+4] = r.x
		f_dest[i+5] = r.y
		f_dest[i+6] = r.z
		
		ind += 1
	
	return f_dest



func desc_( db, index: int ):
	var cmd: String = "SELECT desc FROM data WHERE id = %d LIMIT 1;" % index
	var res: bool = db.query( cmd )
	if not res:
		print( "failed to query frame from the db" )
		return null
	var selected_array : Array = db.query_result
	var stri = selected_array[0]['desc']
	stri = stri.replace( "'", "\"" )
	var ret = JSON.parse( stri )
	ret = ret.result
	return ret


func estimate_scale( db ):
	print( "Estimating scale..." )
	var dims: int = get_config_( db, "desc_length")
	
	db.query("SELECT COUNT(*) AS 'qty' FROM " + TABLE_DATA_NAME + ";")
	var frames_qty: int = db.query_result[0]['qty']

	var fs = FrameSearch.new()
	fs.set_dims( dims )
	
	for i in range( frames_qty ):
		var d: Array = desc_( db, i )
		fs.append( d )
	
	var inv_std: Array  = fs.inv_std()
	var inv_ampl: Array = fs.inv_ampl()
	
	set_config_( db, "inv_std", inv_std )
	set_config_( db, "inv_ampl", inv_ampl )
	
	var desc_lengths = get_config_( db, "desc_lengths" )
	var gains: Array = []
	for i in desc_lengths:
		gains.push_back( 1.0 )
	
	set_config_( db, "desc_gains", gains )
	set_config_( db, "switch_threshold", switch_threshold_ )



func pose_desc_( db, index: int ):
	var f = frame_( db, index )
	var fp
	if ( index > 0 ):
		fp = frame_( db, index-1 )
	else:
		fp = f
	
	# Root pose
	var root_q: Quat = Quat( f[ROOT_IND+1], f[ROOT_IND+2], f[ROOT_IND+3], f[ROOT_IND] )
	var root_r: Vector3 = Vector3( f[ROOT_IND+4], f[ROOT_IND+5], f[ROOT_IND+6] )
	var root_q_inv:    Quat = root_q.inverse()
	var az_root_q:     Quat = quat_azimuth_( root_q )
	var az_root_q_inv: Quat = az_root_q.inverse()
	
	var desc_r: Array
	var desc_v: Array
	for limb_ind in POSE_LIMB_INDS:
		var ind: int = limb_ind * 7
		var q: Quat = Quat( f[ind+1], f[ind+12], f[ind+13], f[ind+10] )
		var r: Vector3 = Vector3( f[ind+4], f[ind+5], f[ind+6] )
		#var qp: Quat = Quat( fp[ind+1], fp[ind+12], fp[ind+13], fp[ind+10] )
		var rp: Vector3 = Vector3( fp[ind+4], fp[ind+5], fp[ind+6] )
		r  -= root_r
		rp -= root_r
		r  = az_root_q_inv.xform( r )
		rp = az_root_q_inv.xform( rp )
		#q  = root_q_inv * q
		#qp = root_q_inv * qp
		var v: Vector3 = (r - rp) * FPS
		if ( limb_ind != ROOT_IND ):
			desc_r += [ r.x, r.y ]
		desc_v += [ v.x, v.y ]
		
	# Also compute Z in local ref. frame. It defines torso tilt.
	#var g: Vector3 = Vector3( 0.0, 0.0, 1.0 )
	#g = root_q_inv.xform( g )
	#var desc_g: Array = [ g.x, g.y ]
	
	#return [ desc_r, desc_v, desc_g ]
	
	return [ desc_r, desc_v ]
	

func traj_desc_( db, index: int ):
	var f = frame_( db, index )
	
	# Root pose
	var root_q: Quat = Quat( f[ROOT_IND+1], f[ROOT_IND+2], f[ROOT_IND+3], f[ROOT_IND] )
	var root_r: Vector3 = Vector3( f[ROOT_IND+4], f[ROOT_IND+5], f[ROOT_IND+6] )
	var root_q_inv = root_q.inverse()
	var az_root_q: Quat = quat_azimuth_( root_q )
	var az_root_q_inv = az_root_q.inverse()
	
	var desc_r: Array
	#var desc_az: Array
	#var desc_g: Array
	
	for ind in TRAJ_FRAME_INDS:
		var frame_ind: int = index + ind
		if frame_ind >= frames_qty_:
			frame_ind = frames_qty_ - 1
		
		var fn = frame_( db, frame_ind )
		#var q: Quat = Quat( fn[ROOT_IND+1], fn[ROOT_IND+2], fn[ROOT_IND+3], fn[ROOT_IND] )
		var r: Vector3 = Vector3( fn[ROOT_IND+4], fn[ROOT_IND+5], fn[ROOT_IND+6] )
		#var q_inv: Quat = q.inverse()
		r = r - root_r
		r = az_root_q_inv.xform( r )
		var d = [r.x, r.y]
		desc_r += d
		
		#var fwd: Vector3 = Vector3( 1.0, 0.0, 0.0 )
		#fwd = q.xform( fwd )
		#fwd = root_q_inv.xform( fwd )
		#d = [fwd.x, fwd.y]
		#desc_az += d
		
		#var g: Vector3 = Vector3( 0.0, 0.0, 1.0 )
		#g =	q_inv.xform( g )
		#d = [g.x, g.y]
		#desc_g += d
		
	#return [desc_r, desc_az, desc_g]
	
	return [desc_r]


func input_based_traj_desc_( db, az_adj_q: Quat, index: int ):
	var f = frame_( db, index )
	
	# Root pose
	var root_q: Quat = Quat( f[ROOT_IND+1], f[ROOT_IND+2], f[ROOT_IND+3], f[ROOT_IND] )
	#var root_r: Vector3 = Vector3( f[ROOT_IND+4], f[ROOT_IND+5], f[ROOT_IND+6] )
	var root_q_inv = root_q.inverse()
	
	var desc_r: Array
	var desc_az: Array
	var desc_g: Array
	
	var rp: Vector2 = Vector2.ZERO
	var fwd: Vector2 = Vector2( 1.0, 0.0 )
	
	var inv_pose_q = pose_q_.inverse()
	for ind in TRAJ_FRAME_INDS:
		var ctrl_ind: int = ind-1
		
		var r: Vector2 = control_pos_sequence_[ctrl_ind]
		var r3 = Vector3( r.x, r.y, 0.0 )
		r3 = inv_pose_q.xform( r3 )
		r = Vector2( r3.x, r3.y )
		var d = [r.x, r.y]
		desc_r += d
		
		var dr: Vector2 = r - rp
		var sz = dr.length_squared()
		if ( sz > 0.00001 ):
			fwd = dr.normalized()
		d = [fwd.x, fwd.y]
		desc_az += d
		
		var g: Vector3 = Vector3( 0.0, 0.0, 1.0 )
		#g = q_inv.xform( g )
		d = [g.x, g.y]
		desc_g += d
		
		rp = r
		
	return [desc_r, desc_az, desc_g]


func init_control_sequence_():
	control_pos_sequence_ = []
	control_vel_sequence_ = []
	var default_ctrl = Vector2(0.0, 0.0)
	var sz = TRAJ_FRAME_INDS.size()
	var qty = TRAJ_FRAME_INDS[sz-1]
	for i in range(qty):
		control_pos_sequence_.push_back( default_ctrl )
		control_vel_sequence_.push_back( default_ctrl )


func frame_azimuth_( db, ind: int ):
	var f = frame_( db, ind )
	var q: Quat = Quat( f[ROOT_IND+1], f[ROOT_IND+2], f[ROOT_IND+3], f[ROOT_IND] )
	q = pose_q_ * q
	var fwd: Vector3 = Vector3( 1.0, 0.0, 0.0 )
	fwd = q.xform( fwd )
	var az: float = atan2( fwd.y, fwd.x )
	return az


func quat_azimuth_( q: Quat ):
	var fwd: Vector3 = Vector3( 1.0, 0.0, 0.0 )
	var q_norm: Quat = q.normalized()
	#print( "quat_qzimuth_(", q, ")" )
	fwd = q_norm.xform( fwd )
	var ang2: float = 0.5 * atan2( fwd.y, fwd.x )
	var co2: float = cos( ang2 )
	var si2: float = sin( ang2 )
	var res_q: Quat = Quat( 0.0, 0.0, si2, co2 )
	return res_q


func generate_controls( t: Transform, f: bool, b: bool, l: bool, r: bool, fast: bool, slow: bool ):
	var cam_q: Quat = t.basis.get_rotation_quat()
	# From Godot ref frame (Y upwards) to normal ref frame (Z upwards).
	cam_q = Quat( cam_q.x, -cam_q.z, cam_q.y, cam_q.w )
	cam_q = quat_azimuth_( cam_q )
	cam_q_ = cam_q
	var fwd: Vector3 = Vector3( 0.0, 0.0, 1.0 )
	
	var ctrl: Vector3 = Vector3( 0.0, 0.0, 0.0 )
	if f:
		ctrl.y += 1.0
	if b:
		ctrl.y -= 1.0
	if l:
		ctrl.x -= 1.0
	if r:
		ctrl.x += 1.0
	
	var speed: float
	if fast:
		speed = fast_speed_
	elif slow:
		speed = slow_speed_
	else:
		speed = walk_speed_
	
	var ln = ctrl.length_squared()
	if ln > 0.5:
		ctrl = ctrl.normalized() * speed
	
	ctrl = cam_q.xform( ctrl )
	var inv_pose_q: Quat = pose_q_.inverse()
	cam_rel_q_ = inv_pose_q * cam_q
	#ctrl = cam_rel_q_.xform( ctrl )
	
	control_input_ = Vector2(ctrl.x, ctrl.y)


func updata_control_sequence_():
	var ctrl: Vector2 = control_input_
	# Shift by one.
	var sz: int = control_vel_sequence_.size()
	for i in range( sz-1 ):
		control_vel_sequence_[i] = control_vel_sequence_[i+1]
	control_vel_sequence_[sz-1] = Vector2( ctrl.x, ctrl.y )
	
	# Re-generate the position sequence.
	var at: Vector2 = Vector2( 0.0, 0.0 )
	for i in range( sz ):
		var v: Vector2 = control_vel_sequence_[i]
		v = v * DT
		at += v
		control_pos_sequence_[i] = at


func update_vis_control_sequence_():
	var qty: int = control_pos_sequence_.size()
	vis_control_sequence_.resize( qty )
	
	#var f = frame_( db_, frame_ind_ )
	#var q: Quat    = Quat( f[ROOT_IND+1], f[ROOT_IND+2], f[ROOT_IND+3], f[ROOT_IND] )
	#var r: Vector3 = Vector3( f[ROOT_IND+4], f[ROOT_IND+5], f[ROOT_IND+6] )
	#var az_q: Quat = quat_azimuth_( q )
	#var inv_az_q = az_q.inverse()
	
	for i in range( qty ):
		var c2: Vector2 = control_pos_sequence_[i]
		var c3: Vector3 = Vector3(c2.x, c2.y, 1.0 )
		#c3 = pose_q_.xform( c3 )
		c3 = c3 + pose_r_
		vis_control_sequence_[i] = c3
	
	
	var ind: int = 0
	qty = TRAJ_FRAME_INDS.size()
	print_control_sequence_.resize( qty )
	var inv_pose_q = pose_q_.inverse()
	for i in TRAJ_FRAME_INDS:
		var v = control_pos_sequence_[i-1]
		var v3 = Vector3( v.x, v.y, 0.0 )
		v3 = inv_pose_q.xform( v3 )
		v = Vector2( v3.x, v3.y )
		print_control_sequence_[ind] = v
		ind += 1



# Assuming time passed is exactly one frame.
func process_frame():
	var time_to_switch: bool
	if switch_counter_ < switch_period_:
		time_to_switch = false
	else:
		time_to_switch = true
		switch_counter_ -= switch_period_
	
	# Increment frame switch counter.
	switch_counter_ += 1
	
	var next_ind: int = frame_ind_ + 1
	if ( next_ind >= frames_qty_ ):
		next_ind = frames_qty_ - 1
	var jump: bool = false
	
	# Update control sequence based on most recent user input.
	updata_control_sequence_()
	# For control sequence visualization.
	update_vis_control_sequence_()
	
	if time_to_switch:
		#var f_cur = frame_( db_, frame_ind_ )
		var desc_p = pose_desc_( db_, frame_ind_ )
		var desc_t = input_based_traj_desc_( db_, pose_q_, frame_ind_ )
		var d = []
		for di in desc_p:
			for v in di:
				d.push_back( v )
		for di in desc_t:
			for v in di:
				d.push_back( v )
		var closest_err: float = frame_search_.nearest( d )
		var closest_ind: int   = frame_search_.nearest_ind()
		if closest_ind != next_ind:
			var next_err: float = frame_search_.dist( d, next_ind )
			var improvement: float = next_err - closest_err
			if improvement >= switch_threshold_:
				next_ind = closest_ind
				jump = true
			
	var fp = frame_( db_, frame_ind_ )
	var fn = frame_( db_, next_ind )
	
	var qp: Quat    = Quat( fp[ROOT_IND+1], fp[ROOT_IND+2], fp[ROOT_IND+3], fp[ROOT_IND] )
	var rp: Vector3 = Vector3( fp[ROOT_IND+4], fp[ROOT_IND+5], fp[ROOT_IND+6] )
	var qn: Quat    = Quat( fn[ROOT_IND+1], fn[ROOT_IND+2], fn[ROOT_IND+3], fn[ROOT_IND] )
	var rn: Vector3 = Vector3( fn[ROOT_IND+4], fn[ROOT_IND+5], fn[ROOT_IND+6] )
	
	#print( "a" )
	var az_qp: Quat = quat_azimuth_( qp )
	#print( "cc" )
	var az_qn: Quat = quat_azimuth_( qn )
	#print( "b" )
	
	var dq: Quat
	var dr: Vector3
	if jump:
		dr = pose_dr_
		dq = pose_dq_
	
	else:
		var inv_az_qp: Quat = az_qp.inverse()
		dq = inv_az_qp * az_qn
		dr = rn - rp
		dr = inv_az_qp.xform( dr )
		dr = pose_q_.xform( dr )
		pose_dr_ = dr
		pose_dq_ = dq
	
	#var rz: float = pose_r_.z
	pose_r_ += dr
	#pose_r_.z = rz
	
	#if abs(dq.z) > 0.015:
	#	print( "Suspicious activity detected at frame # ", frame_ind_ )
	#	print( "fp: ", fp )
	#	print( "fn: ", fn )
	pose_q_ = pose_q_ * dq
	pose_q_ = quat_azimuth_( pose_q_ )
	
	#print( "frame #%d q:(%f, %f, %f, %f), dq:(%f, %f, %f, %f), pq:(%f, %f, %f, %f)" %[ frame_ind_, qn.w, qn.x, qn.y, qn.z, dq.w, dq.x, dq.y, dq.z, pose_q_.w, pose_q_.x, pose_q_.y, pose_q_.z ] )
	
	frame_ind_ = next_ind
	f_ = frame_in_space_( db_, frame_ind_ )



func apply_to_skeleton( s: Skeleton, f: Array ):
	if s == null:
		return
		
	var sz = f.size()
	var bone_ind: int = 0
	var t: Transform
	
	for i in range( 0, sz, 7 ):
		var q: Quat    = Quat( f[i+1], f[i+3], -f[i+2], f[i] )
		var r: Vector3 = Vector3( f[i+4], f[i+6], -f[i+5] )
		
		var ip: int = s.get_bone_parent( bone_ind )
		if ip >= 0:
			var qp: Quat    = Quat( f[ip+1], f[ip+3], -f[ip+2], f[ip] )
			var rp: Vector3 = Vector3( f[ip+4], f[ip+6], -f[ip+5] )
			var inv_qp = qp.inverse()
			q = inv_qp * q
			r = r - rp
			q = Quat.IDENTITY
			r = Vector3.ZERO #inv_qp.xform( r )
		
		t.origin = r
		t.basis  = Basis( q )
		s.set_bone_pose( bone_ind, t )
		bone_ind += 1


func get_parent_skeleton():
	var p = get_parent()
	if p is Skeleton:
		skeleton_ = p
		var qty: int = skeleton_.get_bone_count()
		var t: Transform = Transform.IDENTITY
		for i in range(qty):
			skeleton_.set_bone_rest( i, t )
	else:
		skeleton_ = null


func get_parents_array_():
	if skeleton_ == null:
		return
	
	var qty: int = skeleton_.get_bone_count()
	var parents: Array
	parents.resize( qty )
	for i in range( qty ):
		var ip: int = skeleton_.get_bone_parent( i )
		parents[i] = ip
		
	set_config_( db_, "parents", parents )
	
	return parents
	


func _on_tree_exiting():
	if ( db_ ):
		db_.close_db()
		db_ = null








