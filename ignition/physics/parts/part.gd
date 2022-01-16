
extends PhysicsBodyBase
class_name Part

enum PartClass {
	THRUSTER=0, 
	FUEL_TANK=1, 
	SRB_TANK=2, 
	REACTION_CONTROL_WHEELS=3, 
	REACTION_CONTROL_THRUSTER=4, 
	HABITAT=5, 
	DECOUPLER=6, 
	CHARACTER=7, 
	PASSIVE=8 # This one just means no predefined functionality.
}


enum PartMode {
	CONSTRUCTION=0, 
	SIMULATION=1
}

var PartControlGroups = preload( "res://physics/parts/part_control_groups.gd" )

# If instant velocity change is greater than this, 
# the part should be destroyed.
export(float) var destruction_dv = 5.0
var _last_vel: Vector3 = Vector3.ZERO
var _last_vel_initialized: bool = false
var _force_destroy: bool = false

export(PartClass) var part_class = PartClass.THRUSTER
export(bool) var allows_surface_attachments=true
export(bool) var allows_y_radial_symmetry=true
export(bool) var allows_x_mirror_symmetry=true
export(bool) var conducts_liquid_fuel=true
export(bool) var conducts_solid_fuel=true
export(bool) var conducts_electricity=true
export(bool) var conducts_air=true
# Dry mass of the part. For fuel tanks there should be a separate thing 
# For computing total mass.
export(float) var mass = 1.0

var stacking_nodes: Array = []
var surface_nodes: Array  = []


var mode: int = PartMode.CONSTRUCTION

# 0 = any, <0 = none.
var control_group: int = PartControlGroups.ControlGroup.NONE



var _sound_source: SoundSource = null


# If contains Spatial with prefix "stacking_node" it's position and Y axis direction 
# define the stacking node. Stackig nodes can be coupled with each other.
# If it contains Spatial with prefix "surface_node" defines a surface coupling node. 
# A part can use this node to get attached to another part's surface which allows such 
# conenctions.
func _traverse_coupling_nodes():
	_traverse_coupling_nodes_recursive( _visual )



func _traverse_coupling_nodes_recursive( p: Node ):
	var s: Spatial = p as Spatial
	if s != null:
		var stacking_node: CouplingNode = s as CouplingNode
		var is_stacking_node: bool = (stacking_node != null)
		if is_stacking_node:
			# Specify the reference to itself.
			# Not sure if it is 100% needed but just in case.
			s.part = self
			stacking_nodes.push_back( s )
	
	var qty: int = p.get_child_count()
	for i in range( qty ):
		var ch: Node = p.get_child( i )
		_traverse_coupling_nodes_recursive( ch )


func process_inner( _delta ):
	.process_inner( _delta )
	
	if mode == PartMode.CONSTRUCTION:
		_process_coupling_nodes()
		_process_attachments()
	
	_process_destruction()


func _process_coupling_nodes():
	var qty: int = stacking_nodes.size()
	for i in range(qty):
		var n: CouplingNode = stacking_nodes[i]
		n.process()


func _process_attachments():
	
	var atts: Array = get_attachments()
	var qty: int = atts.size()
	for i in range(qty):
		var n: CouplingAttachment = atts[i]
		n.process()


func _process_destruction():
	var se3: Se3Ref = get_se3()
	if not _last_vel_initialized:
		_last_vel = se3.v
		_last_vel_initialized = true
		return
	
	var dv: Vector3 = se3.v - _last_vel
	var abs_dv: float = dv.length()
	#print( "dv: ", abs_dv )
	if (abs_dv < destruction_dv) and (not _force_destroy):
		_last_vel = se3.v
		return
	
	# Destroying the part and the assembly.
	var sb: Node = get_super_body_raw()
	if (sb != null) and ( is_instance_valid(sb) ):
		sb.queue_free()
	
	self.queue_free()



func _parent_jumped():
	parent_jumped()



func parent_jumped():
	_last_vel_initialized = true
	var se3: Se3Ref = get_se3()
	_last_vel = se3.v



# When being constructed parts are not supposed to move.
# So it is possible to make dynamic bodies kinematic.
# And when editing is done, one can switch those back to 
# being dynamic.
# These two should be overwritten.
func activate( root_call: bool = true ):
	var is_activated: bool = (mode == PartMode.SIMULATION)
	if is_activated:
		return
	
	.activate( root_call )
	
	# Assign mass
	if _physical != null:
		_physical.mass = mass
	
	# If activated, not in construction anymore.
	mode = PartMode.SIMULATION
	
	activate_nodes( true )
	
	# Create super body. Through it controls are computed and 
	# merge/split ref frames are computed.
#	if root_call:
#		var sb = PartAssembly.new()
#		sb.change_parent( RootScene.get_root_for_bodies() )
#		var p = self.get_parent()
#		sb.debug = true
#		sb.change_parent( p )
#		sb.debug = false
#		var se3: Se3Ref = self.get_se3()
#		sb.set_se3( se3 )
#
#		# Add all bodies to this super body.
#		var ret: Array = dfs_search( self )
#		var bodies: Array = ret[1]
#		for body in bodies:
#			sb.add_sub_body( body )
		
		# Now sure what this one is doing.
		#sb.activate()




func deactivate( root_call: bool = true ):
	.deactivate(root_call)
	
	deactivate_nodes( true )
	
	# Need to make sure that not in a construction mode.
	# In construction mode super body is provided externally 
	# And provides additional context menu GUIs.
	if root_call and (mode != PartMode.CONSTRUCTION):
		var sb: Node = get_super_body_raw()
		if sb != null:
			sb.queue_free()
			sb = null




func activate_nodes( activate_parts: bool = false ):
	# Get all connected parts and switch them as well.
	var atts: Array = get_attachments()
	var nodes_qty: int = atts.size()
	if activate_parts:
		for i in range(nodes_qty):
			var n: CouplingAttachment = atts[i]
			var p: Part = n.attachment_b.get_part()
			p.activate( false )
	
	for i in range(nodes_qty):
		var n: CouplingAttachment = atts[i]
		if not n.is_parent:
			continue
		n.activate()


func deactivate_nodes( deactivate_parts: bool = false ):
	# Get all connected parts and switch them as well.
	var atts: Array = get_attachments()
	var nodes_qty: int = atts.size()
	for i in range(nodes_qty):
		var n: CouplingAttachment = atts[i]
		if not n.is_parent:
			continue
		n.deactivate()
	
	if deactivate_parts:
		for i in range(nodes_qty):
			var n: CouplingAttachment = atts[i]
			var p: Part = n.attachment_b.part
			p.deactivate( false )


func set_show_node_visuals( en: bool ):
	for n in stacking_nodes:
		n.show_visual = en




func remove_physical():
	deactivate_nodes( false )
	.remove_physical()



# Checks all the nodes.
# If either one is connected and is not a parent return it here.
func get_coupled_child_node():
	var atts: Array = get_attachments()
	for node in atts:
		var n: CouplingAttachment = node as CouplingAttachment
		var is_parent: bool = n.is_parent
		if is_parent:
			continue
		return n
	
	return null



# Couple with other node.
func couple():
	# First find all bodies attached directly or indirectly to this body 
	# and ignore them in search.
	var direct_bodies: Array = dfs_search( self )
	# Get parent ref frame, get all parts in it.
	var rf: RefFrameNode = get_parent()
	var parts: Array = []
	var qty: int = rf.get_child_count()
	for i in range(qty):
		var ch: Node = rf.get_child( i )
		var p: Part = ch as Part
		if (p != null) and ( not direct_bodies.has(p) ):
			parts.push_back( p )
	
	var parts_qty: int = parts.size()
	
	# Initialize closest nodes with nulls.
	var closest_own_node: CouplingNode   = null
	var closest_other_node: CouplingNode = null
	var closest_distance: float = -1.0
	var other_node_se3: Se3Ref = Se3Ref.new()
	var own_node_se3: Se3Ref   = Se3Ref.new()
	
	var own_nodes_qty: int = stacking_nodes.size()
	for own_node_ind in range(own_nodes_qty):
		var own_node: CouplingNode = stacking_nodes[own_node_ind]
		
		if not own_node.allows_connecting:
			continue
		
		# Assign own SE3.
		own_node_se3.transform = own_node.relative_to_owner
		
		for part_ind in range(parts_qty):
			var part: Part = parts[part_ind]
			if (part == null) or (not is_instance_valid(part)) or (part == self):
				continue
			
			var other_nodes_qty: int = part.stacking_nodes.size()
			for other_node_ind in range(other_nodes_qty):
				var other_node: CouplingNode = part.stacking_nodes[other_node_ind]
				
				# If node does not allows connections to it.
				if not other_node.allows_connections:
					continue
				
				# If it is already connected to the same part, 
				# don't connect it again.
				var already_coupled: bool = self.is_coupled_with( part )
				if already_coupled:
					continue
				
				# Measure the distance between the two nodes.
				other_node_se3.transform = other_node.relative_to_owner
				var se3: Se3Ref = self.relative_to_se3( part, other_node_se3 )
				se3 = se3.mul( own_node_se3 )
				var current_distance: float = se3.r.length()
				if (closest_other_node == null) or (current_distance < closest_distance):
					closest_distance   = current_distance
					closest_own_node   = own_node
					closest_other_node = other_node
	
	if (closest_own_node != null):
		# Try couple the closest nodes.
		var ret: bool = closest_own_node.couple_with( closest_other_node )
		return ret
	
	return false


func couple_surface():
	# First find all bodies attached directly or indirectly to this body 
	# and ignore them in search.
	var direct_bodies: Array = dfs_search( self )
	
	var rf: RefFramePhysics = parent_physics_ref_frame()
	if rf == null:
		return false
	
	var broad_t: BroadTreeGd = rf.get_broad_tree()
	if broad_t == null:
		return false
	
	# This one can be null. It's ok.
	var own_octree: OctreeMeshGd = self.get_octree_mesh()
	
	# Let's find the closest distance.
	var closest_node_ind: int    = -1
	var closest_part_dist: float = -1.0
	var closest_result: Array = []
	
	var own_nodes_qty: int = stacking_nodes.size()
	print( "surface coupling attempt" )
	print( "nodes qty: ", own_nodes_qty )
	for own_node_ind in range(own_nodes_qty):
		var own_node: CouplingNode = stacking_nodes[own_node_ind]
		
		if not own_node.allows_surface_coupling:
			continue
		
		var se3: Se3Ref = own_node.ref_frame_transform()
		var start_r: Vector3 = se3.r
		# x2 because of sphere diameter is x2 of its radius. 
		var sz: float   = own_node.snap_size() * 2.0
		var a: Vector3  = Vector3( 0.0, sz, 0.0 )
		var q: Quat     = se3.q
		a = q.xform( a )
		var end_r: Vector3 = start_r + a
		
		# Returns the closest intersection in the format: 
		# [ ok: bool, 
		#   distance: float, 
		#   intersection_point: Vector3, 
		#   face_center_position: Vector3, 
		#   face_normal: Vector3, 
		#   face_area: float, 
		#   octree_mesh_gd: OctreeMeshGd ]
		var ret: Array = broad_t.intersects_segment_face( start_r, end_r, own_octree )
		print( own_node_ind, " result: ", ret )
		var ok: bool = ret[0]
		if not ok:
			continue
		
		# If intersects with something it is direcrly or indirectly 
		# already connected with, ignore this result.
		var octree_mesh: OctreeMeshGd = ret[6]
		var intersected_part: Part = octree_mesh.get_parent()
		if direct_bodies.has( intersected_part ):
			continue
		
		var distance: float = ret[1]
		if (closest_node_ind < 0) or (distance < closest_part_dist):
			closest_part_dist = distance
			closest_node_ind  = own_node_ind
			closest_result    = ret
	
	if closest_node_ind < 0:
		return false
	
	
	# Here there is a own_node_index and also 
	# there is a part it is the closest to.
	var at: Vector3   = closest_result[2]
	var norm: Vector3 = closest_result[4]
	var other_octree_mesh: OctreeMeshGd = closest_result[6]
	var other_part: Part = other_octree_mesh.get_parent()
	
	var se3: Se3Ref = other_part.get_se3()
	var inv_q0: Quat = se3.q.inverse()
	var r0: Vector3  = se3.r
	at   = inv_q0.xform( at - r0 )
	norm = inv_q0.xform( norm )
	
	var si: float = sqrt(norm.x*norm.x + norm.z*norm.z)
	if si > 1.0:
		si = 1.0
	elif si < -1.0:
		si = -1.0
	var angle_2: float = asin( si ) * 0.5
	var si_2: float = sin( angle_2 )
	var co_2: float = cos( angle_2 )
	
	# Rotation axis is (0, 1, 0).cross( norm )
	var rot_axis: Vector3 = Vector3( norm.z, 0.0, -norm.x )
	rot_axis = rot_axis.normalized()
	
	var q: Quat = Quat( rot_axis.x*si_2, rot_axis.y*si_2, rot_axis.z*si_2, co_2 )
	var t: Transform = Transform.IDENTITY
	t.basis  = q
	t.origin = at
	
	var closest_own_node: CouplingNode = stacking_nodes[closest_node_ind]
	
	var ret: bool = closest_own_node.couple_with_surface( other_part, t )
	return ret





# Removes child connection(s).
# There actually should be just one such connection.
func decouple():
	var atts: Array = get_attachments()
	var couplings_qty: int = atts.size()
	for coupling_ind in range( couplings_qty ):
		var conn: CouplingAttachment = atts[coupling_ind]
		# Don't disconnect child nodes.
		# Only disconnect from a parent.
		if conn.is_parent:
			continue
		var other_conn: CouplingAttachment = conn.attachment_b
		conn.decouple()
		conn.queue_free()
		if other_conn != null:
			other_conn.queue_free()
		return true
	return false



func decouple_all():
	# Decouple all the nodes if the thing is deleted.
	# Well, it is assumed that Part is added to the 
	var atts: Array = get_attachments()
	for n in atts:
		var conn: CouplingAttachment = n
		var other_conn: CouplingAttachment = conn.attachment_b
		conn.decouple()
		conn.queue_free()
		if other_conn != null:
			other_conn.queue_free()



# Need this one to not try to couple two times.
func is_coupled_with( part: Part ):
	var atts: Array = get_attachments()
	var couplings_qty: int = atts.size()
	for coupling_ind in range(couplings_qty):
		var own_node: CouplingAttachment = atts[coupling_ind]
		
		var attachment_b: CouplingAttachment = own_node.attachment_b
		var part_b: Part = attachment_b.get_part()
		
		if part == part_b:
			return true
	
	return false



func update_physics_from_state():
	.update_physics_from_state()
	
#	var t: Transform = self.transform
#	var atts: Array = get_attachments()
#	var qty: int = atts.size()
#	for i in range( qty ):
#		var n: CouplingAttachment = atts[i]
#		n.update_joint_pose( t )




func init():
	.init()
	need_super_body = true
	_traverse_coupling_nodes()



# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()
	_create_sound_source()


func on_delete():
	on_delete_rescue_camera()
	decouple_all()
	.on_delete()



func on_delete_rescue_camera():
	var cam: RefFrameNode = PhysicsManager.camera
	if not is_instance_valid( cam ):
		return
	
	var p: Node = cam.get_parent()
	if p != self:
		return
	
	# This node is being destroyed. If camera is parented to this node, 
	# parent it to the parent of this node.
	p = self.get_parent()
	cam.change_parent( p )


# Depth first search for all the parts connected with each other.
static func dfs_search( p: Part ):
	var parts: Array = []
	_dfs( p, parts )
	
	var ret: Array = parts
	return ret




func get_attachments():
	var qty: int = get_child_count()
	var atts: Array = []
	for i in range(qty):
		var c: Node = get_child(i)
		var ca: CouplingAttachment = c as CouplingAttachment
		if ca != null:
			atts.push_back( ca )
	
	return atts


static func _dfs( part: Part, parts: Array ):
	var already_there: bool = parts.has( part )
	if already_there:
		return
	
	parts.push_back( part )
	
	var atts: Array = part.get_attachments()
	var qty: int = atts.size()
	for i in range(qty):
		var n: CouplingAttachment = atts[i]
		
		var p: Part = n.attachment_b.get_part()
		_dfs( p, parts )



func gui_classes( mode: Array ):
	var classes: Array = .gui_classes( mode )
	if not mode.has( "construction_editing" ):
		# Append with control group selection.
		var gui_control_group = preload( "res://physics/parts/gui_elements/gui_control_group.tscn" )
		classes.push_back( gui_control_group )
		# This is to dorce destruction of the part.
		# I need it for debugging object destruction.
		var gui_destroy = preload( "res://physics/parts/gui_elements/gui_destroy.tscn" )
		classes.push_back( gui_destroy )
	return classes



# Returns if the part should process user input based on its control group.
func control_group_active( input: Dictionary ):
	if control_group == PartControlGroups.ControlGroup.NONE:
		return false
	var has: bool = PartControlGroups.CONTROL_GROUP_NAMES.has(control_group)
	if has:
		var input_name: String = PartControlGroups.CONTROL_GROUP_NAMES[control_group]
		has = input.has( input_name )
		if has:
			return true
	# Now if any of the names exists, and control group is "ANY", than 
	# also return true.
	if control_group == PartControlGroups.ControlGroup.ANY:
		for id in PartControlGroups.CONTROL_GROUP_NAMES:
			var gui_name: String = PartControlGroups.CONTROL_GROUP_NAMES[id]
			has = input.has( gui_name )
			if has:
				return true
	return false



func process_user_input_2( input: Dictionary ):
	var do_process: bool = control_group_active( input )
	if do_process:
		process_user_input_group( input )
	.process_user_input_2( input )


# This one should be overriden by implementations.
func process_user_input_group( input: Dictionary ):
	pass


# This one is supposed to override the dummy one 
# declared in "PhysicsBodyBase".
func create_super_body():
	var p = self.get_parent()
	var sb = PartAssembly.new()
	sb.debug = true
	sb.change_parent( p )
	sb.debug = false
	var se3: Se3Ref = self.get_se3()
	sb.set_se3( se3 )
	
	# Add all bodies to this super body.
	var bodies: Array = dfs_search( self )
	for body in bodies:
		sb.add_sub_body( body )
	
	return sb



func serialize():
	var data: Dictionary = .serialize()
	data["mode"]          = int(mode)
	data["control_group"] = int(control_group)
	
	var atts: Array = get_attachments()
	# Need to serialize all the nodes.
	var coupling_nodes_data: Dictionary = {}
	for n in atts:
		var node: CouplingAttachment = n
		var node_data: Dictionary = node.serialize()
		var name: String = node.name
		coupling_nodes_data[name] = node_data
	data["coupling_attachments"] = coupling_nodes_data
	
	return data



func deserialize( data: Dictionary ):
	# Reset body state to something impossible to force it to initialize.
	body_state = -1
	
	# It is set to the right value in activation inside of the Body.deserialize( data ).
	#var new_mode: int = data["mode"]
	control_group = data["control_group"]
	
	# It's here. Need to move coupling attachments creation to bodies creation place.
	# The thing is whenever a coupling attachment is deserialized, its mate 
	# might not exist yet.
	# Need to deserialize all the nodes.
	var coupling_nodes_data: Dictionary = data["coupling_attachments"]
	for name in coupling_nodes_data:
		var att: CouplingAttachment = get_node( name )
		if att == null:
			continue
		var att_data: Dictionary = coupling_nodes_data[name]
		att.deserialize( att_data )
	
	var ret: bool = .deserialize( data )
	if not ret:
		return false
	
	return true



static func create_attachments( part: Part, data: Dictionary ):
	# Need to deserialize all the nodes.
	var coupling_nodes_data: Dictionary = data["coupling_attachments"]
	for name in coupling_nodes_data:
		var node_data: Dictionary = coupling_nodes_data[name]
		var n: CouplingAttachment = CouplingAttachment.new()
		n.name = name
		part.add_child( n )



func is_character():
	var ret: bool = (part_class == PartClass.CHARACTER)
	return ret



func _create_sound_source():
	var s: SoundSource = SoundSource.new()
	add_child( s )
	# Make sure that the displacement is Identity.
	var se3: Se3Ref = Se3Ref.new()
	s.set_se3( se3 )


func _get_sound_source():
	if _sound_source == null:
		var qty: int = get_child_count()
		for i in range(qty):
			var c: Node = get_child( i )
			var ss: SoundSource = c as SoundSource
			if ss != null:
				_sound_source = ss
				break
	
	return _sound_source



func play_sound( main: String, start: String = "", stop: String = "" ):
	var ss: SoundSource = _get_sound_source() 
	ss.play( main, start, stop )



func stop_sound( sound: String ):
	var ss: SoundSource = _get_sound_source()
	ss.stop( sound )



func stop_all_sounds():
	var ss: SoundSource = _get_sound_source()
	ss.stop_all()




