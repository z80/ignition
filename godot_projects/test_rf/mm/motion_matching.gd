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
var switch_period_: int = 15

const ROOT_IND: int       = 1
const LEFT_LEG_IND: int   = 81
const RIGHT_LEG_IND: int  = 86
const LEFT_HAND_IND: int  = 8
const RIGHT_HAND_IND: int = 12
#const POSE_LIMB_INDS: Array = [ROOT_IND, LEFT_LEG_IND, RIGHT_LEG_IND, LEFT_HAND_IND, RIGHT_HAND_IND]
#const TRAJ_FRAME_INDS: Array = [30, 60, 90, 120]
const POSE_LIMB_INDS: Array = [ROOT_IND, LEFT_LEG_IND, RIGHT_LEG_IND]
const TRAJ_FRAME_INDS: Array = [10, 20, 30, 40]

const FPS: float = 15.0
const DT: float  = 1.0/FPS


# Forward vector in local ref frame.
const V_HEADING_FWD: Vector3       = Vector3( 0.0, -1.0, 0.0 )


# Movement constants for building future trajectory.
var slow_speed_: float = 0.5
var walk_speed_: float = 0.7
var fast_speed_: float = 2.0

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


# Run the algorithm, or just demonstrate frames.
var run_mm_algorithm_:    bool = true
var increment_frame_ind_: bool = true

# Called when the node enters the scene tree for the first time.
func _ready():
	# Remember! After assigning categoriesit is necessary to re-generate descriptors (!!!)
	# So the work pattern is 
	# 1) generate_descriptors( true, true )
	# 2) Assign categories.
	# 3) generate_descriptors( false, true )
	
	#generate_descriptors( true, true )
	#generate_descriptors( false, true )
	init()


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
	var db = open_frame_database_()
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
	
	#var sp = get_config_( db, "switch_period" )
	#if sp == null:
	#	switch_period_ = 5
	#	set_config_( db, "switch_period", switch_period_ )
	#else:
	#	switch_period_ = sp
	
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
	
	var ps = get_config_( db_, "parent_inds" )
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
func generate_descriptors( fill_categories: bool = false, fill_descs: bool = true ):
	var db = open_frame_database_()
	create_desc_column_( db )
	create_cat_column_( db )
	if fill_categories:
		fill_default_cats_( db )
	if fill_descs:
		fill_descs_( db )
		fill_desc_lengths_( db )
		estimate_scale_( db )
	
	fill_desc_lengths_( db )
	
	db.close_db()


func open_frame_database_():
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
	for i in range(bone_names_.size()):
		print( "%d: %s" % [i, bone_names_[i]] )
	
	return db


func has_desc_column_( db ):
	var cmd: String = "SELECT COUNT(*) AS qty FROM pragma_table_info('desc') WHERE name='data';"
	var res: bool = db.query( cmd )
	var qty: int = db.query_result[0]["qty"]
	var has_desc: bool = (qty > 0)
	#print( "query result: ", has_desc )
	return has_desc


func create_desc_column_( db ):
	var cmd: String = "ALTER TABLE data ADD COLUMN desc text;"
	var res: bool = db.query( cmd )
	if not res:
		print( "failed to add column \'desc\' to the table \'data\'" )
		return false
	return true


func create_cat_column_( db ):
	var cmd: String = "ALTER TABLE data ADD COLUMN cat text;"
	var res: bool = db.query( cmd )
	if not res:
		print( "failed to add column \'cat\' to the table \'data\'" )
		return false
	return true


func fill_descs_( frame_db ):
	# Number of frames
	frame_db.query("SELECT COUNT(*) AS 'qty' FROM " + TABLE_DATA_NAME + ";")
	var frames_qty: int = frame_db.query_result[0]['qty']
	
	var pose_desc: Array
	var traj_desc: Array
	var descs: Array
	var desc: Array
	
	for i in range( frames_qty ):
		#f = frame_( frame_db, i )
		pose_desc = pose_desc_( frame_db, i )
		traj_desc = traj_desc_( frame_db, i )
		descs = pose_desc + traj_desc
		desc = []
		for d in descs:
			desc += d
		
		#var stri_f: String = JSON.print( f )
		#stri_f = stri_f.replace( "\"", "\'" )
		var stri_d: String = JSON.print( desc )
		stri_d = stri_d.replace( "\"", "\'" )
		#var cmd = "INSERT OR REPLACE INTO data(id, data, desc) VALUES(%d, \'%s\', \'%s\');" % [i, stri_f, stri_d]
		var cmd = "UPDATE data SET desc=\'%s\' WHERE id=%d;" % [stri_d, i]
		var res: bool = frame_db.query( cmd )
		if not res:
			print( "Failed to write into desc_db" )
			return false
		
	return true


func fill_desc_lengths_( db ):
	# Write figure out descriptor lengths
	#var f = frame_( db, 0 )
	var pose_desc = pose_desc_( db, 0 )
	var traj_desc = traj_desc_( db, 0 )
	var descs: Array = pose_desc + traj_desc
	var desc: Array = []
	var desc_lengths: Array = []
	for d in descs:
		var sz = d.size()
		desc_lengths.push_back( sz )
		desc += d
	var desc_length: int = desc.size()
	
	set_config_( db, "desc_length",  desc_length )
	set_config_( db, "desc_lengths", desc_lengths )
	
	var gains: Array = []
	for i in desc_lengths:
		gains.push_back( 1.0 )
	
	set_config_( db, "desc_gains", gains )
	set_config_( db, "switch_threshold", switch_threshold_ )


func fill_default_cats_( db ):
	# Number of frames
	#db.query("SELECT COUNT(*) AS 'qty' FROM " + TABLE_DATA_NAME + ";")
	#var frames_qty: int = db.query_result[0]['qty']

	var stri_d: String = JSON.print( [0] )
	stri_d = stri_d.replace( "\"", "\'" )
	var cmd = "UPDATE data SET cat=\'%s\' WHERE id>=0;" % [stri_d]
	var res: bool = db.query( cmd )
	if not res:
		print( "Failed to write default value into category " )
		return false
	
	return true


func assign_cat( from: int, to: int, category: int ):
	var stri_d: String = JSON.print( [category] )
	stri_d = stri_d.replace( "\"", "\'" )
	var cmd = "UPDATE data SET cat=\'%s\' WHERE id>=%d AND id <=%d;" % [stri_d, from, to]
	var res: bool = db_.query( cmd )
	if not res:
		print( "Failed to write value into category column" )
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


func cat_desc_( db, index: int ):
	var cmd: String = "SELECT cat FROM data WHERE id = %d LIMIT 1;" % index
	var res: bool = db.query( cmd )
	if not res:
		print( "failed to query frame from the db" )
		return null
	var selected_array : Array = db.query_result
	var stri = selected_array[0]['cat']
	stri = stri.replace( "'", "\"" )
	var ret = JSON.parse( stri )
	ret = ret.result
	return ret


func frame_in_space_( db, index: int ):
	var f = frame_( db, index )
	var sz: int = f.size()
	var f_dest: Array = []
	f_dest.resize( sz )
	
	var root_ind: int = ROOT_IND*7
	var q_root: Quat = Quat( f[root_ind+1], f[root_ind+2], f[root_ind+3], f[root_ind] )
	var r_root: Vector3 = Vector3( f[root_ind+4], f[root_ind+5], f[root_ind+6] )
	
	var base_q_root = quat_azimuth_( q_root )
	var inv_base_q_root = base_q_root.inverse()
	var q_adj = pose_q_ * inv_base_q_root
	
	var ind: int = 0
	for i in range( 0, sz, 7 ):
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
	var json_desc = JSON.parse( stri )
	
	var d = json_desc.result
	
	return d


func estimate_scale_( db ):
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


func pose_desc_( db, index: int ):
	var f = frame_( db, index )
	var fp
	if ( index > 0 ):
		fp = frame_( db, index-1 )
	else:
		fp = f
	
	# Root pose
	var root_ind: int = ROOT_IND*7
	var root_q: Quat = Quat( f[root_ind+1], f[root_ind+2], f[root_ind+3], f[root_ind] )
	var root_r: Vector3 = Vector3( f[root_ind+4], f[root_ind+5], f[root_ind+6] )
	var root_q_inv:    Quat = root_q.inverse()
	var az_root_q:     Quat = quat_azimuth_( root_q )
	var az_root_q_inv: Quat = az_root_q.inverse()
	
	var desc_r: Array
	var desc_v: Array
	var desc_c: Array
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
		#q  = root_q_inv * q
		#qp = root_q_inv * qp
		var v: Vector3 = (r - rp) * FPS
		if ( limb_ind != ROOT_IND ):
			desc_r += [ r.x, r.y ]
		else:
			desc_r.push_back( root_r.z )
		desc_v += [ v.x, v.y ]
		
	# Also compute Z in local ref. frame. It defines torso tilt.
	#var g: Vector3 = Vector3( 0.0, 0.0, 1.0 )
	#g = root_q_inv.xform( g )
	#var desc_g: Array = [ g.x, g.y ]
	
	desc_c = cat_desc_( db, index )
	
	#return [ desc_r, desc_v, desc_g ]
	
	return [ desc_r, desc_v, desc_c ]
	

func traj_desc_( db, index: int ):
	var f = frame_( db, index )
	
	# Root pose
	var root_ind: int = ROOT_IND*7
	var root_q: Quat = Quat( f[root_ind+1], f[root_ind+2], f[root_ind+3], f[root_ind] )
	var root_r: Vector3 = Vector3( f[root_ind+4], f[root_ind+5], f[root_ind+6] )
	var root_q_inv = root_q.inverse()
	var az_root_q: Quat = quat_azimuth_( root_q )
	var az_root_q_inv = az_root_q.inverse()
	
	var desc_r: Array = []
	var desc_z: Array = []
	var desc_heading: Array = []
	#var desc_az: Array
	#var desc_g: Array
	var desc_c: Array = []
	
	for ind in TRAJ_FRAME_INDS:
		var frame_ind: int = index + ind
		if frame_ind >= frames_qty_:
			frame_ind = frames_qty_ - 1
		
		var fn = frame_( db, frame_ind )
		var q: Quat = Quat( fn[root_ind+1], fn[root_ind+2], fn[root_ind+3], fn[root_ind] )
		var r: Vector3 = Vector3( fn[root_ind+4], fn[root_ind+5], fn[root_ind+6] )
		var az_q: Quat = quat_azimuth_( q )
		var r_z: float = r.z
		r = r - root_r
		r = az_root_q_inv.xform( r )
		var d = [r.x, r.y]
		desc_r += d
		desc_z.push_back( r_z )
		
		var fwd: Vector3 = V_HEADING_FWD
		fwd = az_q.xform( fwd )
		fwd = az_root_q_inv.xform( fwd )
		desc_heading += [fwd.x, fwd.y]
		
		var c = cat_desc_( db, frame_ind )
		desc_c += c
		
	#return [desc_r, desc_az, desc_g]
	
	return [desc_r, desc_z, desc_heading, desc_c]


func input_based_pose_desc_( db, index: int, cat: Array = [0] ):
	var f = frame_( db, index )
	var fp
	if ( index > 0 ):
		fp = frame_( db, index-1 )
	else:
		fp = f
	
	# Root pose
	var root_ind: int = ROOT_IND*7
	var root_q: Quat = Quat( f[root_ind+1], f[root_ind+2], f[root_ind+3], f[root_ind] )
	var root_r: Vector3 = Vector3( f[root_ind+4], f[root_ind+5], f[root_ind+6] )
	var root_q_inv:    Quat = root_q.inverse()
	var az_root_q:     Quat = quat_azimuth_( root_q )
	var az_root_q_inv: Quat = az_root_q.inverse()
	
	var desc_r: Array = []
	var desc_v: Array = []
	var desc_c: Array = []
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
		#q  = root_q_inv * q
		#qp = root_q_inv * qp
		var v: Vector3 = (r - rp) * FPS
		if ( limb_ind != ROOT_IND ):
			desc_r += [ r.x, r.y ]
		else:
			desc_r.push_back( root_r.z )
		desc_v += [ v.x, v.y ]
		
	# Also compute Z in local ref. frame. It defines torso tilt.
	#var g: Vector3 = Vector3( 0.0, 0.0, 1.0 )
	#g = root_q_inv.xform( g )
	#var desc_g: Array = [ g.x, g.y ]
	
	desc_c = cat
	
	#return [ desc_r, desc_v, desc_g ]
	
	return [ desc_r, desc_v, desc_c ]


func input_based_traj_desc_( db, category: int = 0 ):
	var f = frame_( db, frame_ind_ )
	var root_ind: int   = ROOT_IND * 7
	var root_r_z: float = f[root_ind+6]
	#var q_frame_az: Quat = frame_azimuth_( db, frame_ind_ )
	
	# Skeleton orientation q = pose_q_ * q_frame_az.inverse()
	# Inverted one is inv_q = q_frame_az * pose_q_.inverse()
	
	#var inv_q: Quat = q_frame_az * pose_q_.inverse()
	
	var desc_r: Array = []
	var desc_z: Array = []
	var desc_heading: Array = []
	#var desc_az: Array
	#var desc_g: Array
	var desc_c: Array = []
	
	var inv_pose_q = pose_q_.inverse()
	for ind in TRAJ_FRAME_INDS:
		var ctrl_ind: int = ind
		
		var r: Vector2 = control_pos_sequence_[ctrl_ind]
		var r3 = Vector3( r.x, r.y, 0.0 )
		r3 = inv_pose_q.xform( r3 )
		r = Vector2( r3.x, r3.y )
		var d = [r.x, r.y]
		desc_r += d
		
		desc_z.push_back( root_r_z )
		
		desc_c.push_back( category )
	
	
	var fwd = V_HEADING_FWD
	var qty: int = control_vel_sequence_.size()
	for ind in range( qty ):
		var v: Vector2 = control_vel_sequence_[ind]
		var len2: float = v.length_squared()
		if len2 > 0.0001:
			var v3: Vector3 = Vector3( v.x, v.y, 0.0 )
			v3 = inv_pose_q.xform( v3 )
			v3 = v3.normalized()
			fwd.x = v3.x
			fwd.y = v3.y
		if ind in TRAJ_FRAME_INDS:
			desc_heading += [ fwd.x, fwd.y ]
	
	return [desc_r, desc_z, desc_heading, desc_c]


func init_control_sequence_():
	control_pos_sequence_ = []
	control_vel_sequence_ = []
	var default_ctrl = Vector2(0.0, 0.0)
	var sz = TRAJ_FRAME_INDS.size()
	var qty = TRAJ_FRAME_INDS[sz-1]+1
	for i in range(qty):
		control_pos_sequence_.push_back( default_ctrl )
		control_vel_sequence_.push_back( default_ctrl )


func frame_azimuth_( db, ind: int ):
	var f = frame_( db, ind )
	var root_ind: int = ROOT_IND*7
	var q: Quat = Quat( f[root_ind+1], f[root_ind+2], f[root_ind+3], f[root_ind] )
	var az_q: Quat = quat_azimuth_( q )
	return az_q


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


func updata_control_sequence_( cat: int = 0 ):
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
	var inv_pose_q: Quat = pose_q_.inverse()
	var q_frame_az: Quat = frame_azimuth_( db_, frame_ind_ )
	var q: Quat = q_frame_az * inv_pose_q
	for i in TRAJ_FRAME_INDS:
		var v = control_pos_sequence_[i-1]
		var v3 = Vector3( v.x, v.y, 0.0 )
		v3 = q.xform( v3 )
		v = Vector2( v3.x, v3.y )
		print_control_sequence_[ind] = v
		ind += 1





# Assuming time passed is exactly one frame.
func process_frame():
	var time_to_switch: bool
	
	if run_mm_algorithm_:
		if switch_counter_ < switch_period_:
			time_to_switch = false
		else:
			time_to_switch = true
			switch_counter_ -= switch_period_
		
		# Increment frame switch counter.
		switch_counter_ += 1
	else:
		time_to_switch = false
	
	
	var next_ind: int = frame_ind_
	if increment_frame_ind_:
		next_ind += 1
		if ( next_ind >= frames_qty_ ):
			next_ind = frames_qty_ - 1
	
	
	var jump: bool = false
	
	# Update control sequence based on most recent user input.
	updata_control_sequence_()
	# For control sequence visualization.
	update_vis_control_sequence_()
	
	if time_to_switch:
		#var f_cur = frame_( db_, frame_ind_ )
		var desc_p = input_based_pose_desc_( db_, frame_ind_ )
		var desc_t = input_based_traj_desc_( db_ )
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
	
	var root_ind: int = ROOT_IND*7
	var qp: Quat    = Quat( fp[root_ind+1], fp[root_ind+2], fp[root_ind+3], fp[root_ind] )
	var rp: Vector3 = Vector3( fp[root_ind+4], fp[root_ind+5], fp[root_ind+6] )
	var qn: Quat    = Quat( fn[root_ind+1], fn[root_ind+2], fn[root_ind+3], fn[root_ind] )
	var rn: Vector3 = Vector3( fn[root_ind+4], fn[root_ind+5], fn[root_ind+6] )
	
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
	
	# To make it not go up or down.
	if jump:
		pose_r_.z = 0.0
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



func _on_tree_exiting():
	if ( db_ ):
		db_.close_db()
		db_ = null








