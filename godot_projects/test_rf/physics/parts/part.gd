
extends Body
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
		var stacking_node: CouplingNodeStacking = s as CouplingNodeStacking
		var is_stacking_node: bool = (stacking_node != null)
		var is_surface_node: bool = false
		if not is_stacking_node:
			var surface_node: CouplingNodeSurface = s as CouplingNodeSurface
			is_surface_node = (surface_node != null)
		if is_stacking_node or is_surface_node:
			# Specify the reference to itself.
			# Not sure if it is 100% needed but just in case.
			s.part = self
			if is_stacking_node:
				stacking_nodes.push_back( s )
			else:
				surface_nodes.push_back( s )
	
	var qty: int = p.get_child_count()
	for i in range( qty ):
		var ch: Node = p.get_child( i )
		_traverse_coupling_nodes_recursive( ch )


func process_inner( _delta ):
	.process_inner( _delta )
	var qty: int = stacking_nodes.size()
	for i in range(qty):
		var n: CouplingNodeStacking = stacking_nodes[i]
		n.process()
	
	process_destruction()



func process_destruction():
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
	var nodes_qty: int = stacking_nodes.size()
	if activate_parts:
		for i in range(nodes_qty):
			var n: CouplingNodeStacking = stacking_nodes[i]
			var c: bool = n.connected()
			if not c:
				continue
			var p: Part = n.node_b.part
			p.activate( false )
	
	for i in range(nodes_qty):
		var n: CouplingNodeStacking = stacking_nodes[i]
		var c: bool = n.connected()
		if not c:
			continue
		if not n.is_parent:
			continue
		n.activate()


func deactivate_nodes( deactivate_parts: bool = false ):
	# Get all connected parts and switch them as well.
	var nodes_qty: int = stacking_nodes.size()
	for i in range(nodes_qty):
		var n: CouplingNodeStacking = stacking_nodes[i]
		var c: bool = n.connected()
		if not c:
			continue
		if not n.is_parent:
			continue
		n.deactivate()
	
	if deactivate_parts:
		for i in range(nodes_qty):
			var n: CouplingNodeStacking = stacking_nodes[i]
			var c: bool = n.connected()
			if not c:
				continue
			var p: Part = n.part_b.part
			p.deactivate( false )


func set_show_node_visuals( en: bool ):
	for n in stacking_nodes:
		n.show_visual = en
	for n in surface_nodes:
		n.show_visual = en




func remove_physical():
	deactivate_nodes( false )
	.remove_physical()



# Checks all the nodes.
# If either one is connected and is not a parent return it here.
func get_coupled_child_node():
	for node in stacking_nodes:
		var n: CouplingNodeStacking = node as CouplingNodeStacking
		var is_connected: bool = n.connected()
		if not is_connected:
			continue
		var is_parent: bool = n.is_parent
		if is_parent:
			continue
		return n
	
	return null




func couple():
	# Get parent ref frame, get all parts in it.
	var rf: RefFrameNode = get_parent()
	var parts: Array = []
	var qty: int = rf.get_child_count()
	for i in range(qty):
		var ch: Node = rf.get_child( i )
		var p: Part = ch as Part
		if (p != null) and (p != self):
			parts.push_back( p )
	
	var parts_qty: int = parts.size()
	
	var done: bool = false
	
	# Initialize closest nodes with nulls.
	var closest_own_node: CouplingNodeStacking   = null
	var closest_other_node: CouplingNodeStacking = null
	var closest_distance: float = -1.0
	var other_node_se3: Se3Ref = Se3Ref.new()
	var own_node_se3: Se3Ref   = Se3Ref.new()
	
	var own_nodes_qty: int = stacking_nodes.size()
	for own_node_ind in range(own_nodes_qty):
		var own_node: CouplingNodeStacking = stacking_nodes[own_node_ind]
		# Skip the already coupled ones.
		if own_node.connected():
			continue
		
		# Assign own SE3.
		own_node_se3.transform = own_node.relative_to_owner
		
		for part_ind in range(parts_qty):
			var part: Part = parts[part_ind]
			if (part == null) or (not is_instance_valid(part)) or (part == self):
				continue
			
			var other_nodes_qty: int = part.stacking_nodes.size()
			for other_node_ind in range(other_nodes_qty):
				var other_node: CouplingNodeStacking = part.stacking_nodes[other_node_ind]
				# If already coupled, skip it.
				if other_node.connected():
					continue
				
				# Mesure the distance between the two nodes.
				other_node_se3.transform = other_node.relative_to_owner
				var se3: Se3Ref = self.relative_to_se3( part, other_node_se3 )
				se3 = se3.mul( own_node_se3 )
				var current_distance: float = se3.r.length()
				if (closest_other_node == null) or (closest_distance < current_distance):
					closest_distance   = current_distance
					closest_own_node   = own_node
					closest_other_node = other_node
	
	if (closest_own_node != null):
		# Try couple the closest nodes.
		var ret: bool = closest_own_node.couple_with( closest_other_node )
		return ret
	
	return false



func decouple():
	var own_nodes_qty: int = stacking_nodes.size()
	for own_node_ind in range( own_nodes_qty ):
		var own_node: CouplingNodeStacking = stacking_nodes[own_node_ind]
		# Don't disconnect child nodes.
		# Only disconnect from a parent.
		if (own_node.node_b != null) and (own_node.is_parent):
			continue
		own_node.decouple()
		return true
	return false



func decouple_all():
	# Decouple all the nodes if the thing is deleted.
	# Well, it is assumed that Part is added to the 
	for n in stacking_nodes:
		var node: CouplingNodeStacking = n
		node.decouple()



func update_physics_from_state():
	.update_physics_from_state()
	
	var t: Transform = self.transform
	var qty: int = stacking_nodes.size()
	for i in range( qty ):
		var n: CouplingNodeStacking = stacking_nodes[i]
		n.update_joint_pose( t )




func init():
	.init()
	need_super_body = true
	_traverse_coupling_nodes()



# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()


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
	var root_part: Part = _find_root( p )
	var parts: Array = []
	_dfs( root_part, parts )
	
	var ret: Array = [ root_part, parts ]
	return ret




# Called when the node enters the scene tree for the first time.
# It's not formally needed to start with root.
# It is only done for extra convenience to know what part is the root.
static func _find_root( p: Part ):
	var part: Part = p
	var again: bool = true
	while again:
		again = false
		# Check all the connected nodes.
		var qty: int = part.surface_nodes.size()
		for i in range(qty):
			var n: CouplingNodeSurface = part.surface_nodes[i]
			var connected: bool = n.connected()
			if not connected:
				continue
			var is_parent: bool = n.is_parent
			if is_parent:
				continue
			# Switch to this node's part.
			part = n.part
			again = true
			break
	return p



static func _dfs( part: Part, parts: Array ):
	var already_there: bool = parts.has( part )
	if already_there:
		return
	
	parts.push_back( part )
	
	var qty: int = part.stacking_nodes.size()
	for i in range(qty):
		var n: CouplingNodeStacking = part.stacking_nodes[i]
		var connected: bool = n.connected()
		if not connected:
			continue
		var is_parent: bool = n.is_parent
		if not is_parent:
			continue
		
		var p: Part = n.node_b.part
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
# declared in "Body".
func create_super_body():
	var p = self.get_parent()
	var sb = PartAssembly.new()
	sb.debug = true
	sb.change_parent( p )
	sb.debug = false
	var se3: Se3Ref = self.get_se3()
	sb.set_se3( se3 )
	
	# Add all bodies to this super body.
	var ret: Array = dfs_search( self )
	var bodies: Array = ret[1]
	for body in bodies:
		sb.add_sub_body( body )
	
	return sb



func serialize():
	var data: Dictionary = .serialize()
	data["mode"]          = int(mode)
	data["control_group"] = int(control_group)
	
	# Need to serialize all the nodes.
	var stacking_nodes_data: Array = []
	for n in stacking_nodes:
		var node: CouplingNodeStacking = n
		var node_data: Dictionary = node.serialize()
		stacking_nodes_data.push_back( node_data )
	data["stacking_nodes"] = stacking_nodes_data
	
	return data



func deserialize( data: Dictionary ):
	# Reset body state to something impossible to force it to initialize.
	body_state = -1
	
	# It is set to the right value in activation inside of the Body.deserialize( data ).
	#var new_mode: int = data["mode"]
	control_group = data["control_group"]
	
	#init()
	# Need to deserialize all the nodes.
	var stacking_nodes_data: Array = data["stacking_nodes"]
	var qty: int = stacking_nodes.size()
	for i in range( qty ):
		var node_data: Dictionary = stacking_nodes_data[i]
		var n = stacking_nodes[i]
		n.deserialize( node_data )
	
	var ret: bool = .deserialize( data )
	if not ret:
		return false
	
	return true


func is_character():
	var ret: bool = (part_class == PartClass.CHARACTER)
	return ret


