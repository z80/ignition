
extends RefFrameBodyNode
class_name PhysicsBodyBase

enum BodyState {
	PASSIVE=0,  # Means notdriven by physical state.
	ACTIVE=1,   # Means driven by physical state.
	CONSTRUCTION=2
}

@export var body_state: BodyState         = BodyState.ACTIVE

# When inheriting need to redefine these two.
@export var VisualScene: PackedScene   = null
@export var AirDragScene: PackedScene  = null



var _visual: RigidBody3D       = null
var _physical: RigidBody3D     = null
var _air_drag_scene: Node    = null
var _shock_wave_visual: Node = null


# Body which contains this one and other bodies.
# When setter and getter are allowed simultaneously it falls into infinite recursion which 
# can not be stopped even by the debugger.
var _assembly: Node = null


var _octree_mesh: OctreeMeshGd = null


# Force visualizer
var force: Node3D = null


#func get_class():
#	return "PhysicsBodyBase"





func _enter_tree():
	#var file_name: String = self.filename
	#var node_path: String = self.get_path()
	#print( "_enter_tree called on PhysicsBodyBase; " + file_name + "; path: " + node_path )
	create_physical()



func _exit_tree():
	#var file_name: String = self.filename
	#var node_path: String = self.get_path()
	#print( "_exit_tree called on PhysicsBodyBase; " + file_name + "; path: " + node_path )
	remove_physical()
	
	var to_be_deleted: bool = is_queued_for_deletion()
	if to_be_deleted:
		on_delete()
		#print( "deleted " + file_name + "; path: " + node_path )



func _ready():
	add_to_group( Constants.BODIES_GROUP_NAME )
	
	if AirDragScene != null:
		_create_octree_mesh()
		# Create air drag mesh(es).
		_air_drag_scene = AirDragScene.instantiate()
		_octree_mesh.add_child( _air_drag_scene )
		_air_drag_scene.visible = false
		_octree_mesh.rebuild()
	
	# Force visualizer. This is purely for debugging purposes.
	if Constants.DEBUG and (force == null) and false:
		var Force = preload( "res://physics/force_source/force_visualizer.tscn" )
		force = Force.instantiate()
		RootScene.get_visual_layer_near().add_child( force )
	
	if _visual == null:
		init()



func _parent_jumped():
	var parent_rf = _parent_physics_ref_frame()
	if parent_rf == null:
		remove_physical()
	else:
		update_physics_from_state()


func _set_se3( se3 ):
	if (_physical != null) and (is_instance_valid(_physical)):
		_physical.transform        = se3.transform
		_physical.linear_velocity  = se3.v
		_physical.angular_velocity = se3.w


func init():
	create_visual()







func _traverse_interaction_nodes():
	var _ok: bool = _traverse_interation_nodes_recursive( _visual )
	



func _traverse_interation_nodes_recursive( p: Node ):
	if p == null:
		return false
	
	var s: Node3D = p as Node3D
	if s != null:
		var node: InteractionNode = s as InteractionNode
		if node != null:
			# Specify the reference to itself.
			node.target = self
			return true
	
	var qty: int = p.get_child_count()
	for i in range( qty ):
		var ch: Node = p.get_child( i )
		var ok: bool = _traverse_interation_nodes_recursive( ch )
		if ok:
			return true
	
	return false



func _traverse_shock_wave_visuals():
	_traverse_shock_wave_visuals_recursive( _visual )


func _traverse_shock_wave_visuals_recursive( node: Node ):
	if node == null:
		return
	
	var s: Node3D = node as Node3D
	if s != null:
		var swv: ShockWaveVisual = s as ShockWaveVisual
		if swv != null:
			# Specify the reference to itself.
			_shock_wave_visual = swv
			return true
	
	var qty: int = node.get_child_count()
	for i in range( qty ):
		var ch: Node = node.get_child( i )
		var ret: bool = _traverse_shock_wave_visuals_recursive( ch )
		if ret:
			return true
	
	return false



# The overrideable version without "_" prefix.
func on_delete():
	var sb: Node = get_assembly_raw()

	if is_instance_valid( sb ):
		sb.remove_sub_body( self )
	if force != null:
		force.name = force.name + "_to_be_deleted"
		force.queue_free()
		force = null
	
	if _visual != null:
		_visual.name = _visual.name + "_to_be_deleted"
		_visual.queue_free()
		_visual = null
	
	if _physical != null:
		_physical.name = _physical.name + "_to_be_deleted"
		_physical.queue_free()
		_physical = null













func update_visual( origin: RefFrameNode = null ):
	if _visual != null:
		var se3: Se3Ref = self.relative_to( origin )
		var t: Transform3D = se3.transform
		_visual.transform = t


# Need to overload this in order to apply controls
func update_physical( _delta: float ):
	pass



# Should be called by a planet if this thing is in the atmosphere.
func update_shock_wave_visual( broad_tree: BroadTreeGd, otree_meshes: Array, density: float, vel_in_mesh: Vector3, se3_mesh_to_rf: Se3Ref, se3_rf_to_mesh: Se3Ref ):
	if _shock_wave_visual == null:
		return
	
	_shock_wave_visual.draw_shock_wave( broad_tree, otree_meshes, density, vel_in_mesh, se3_mesh_to_rf, se3_rf_to_mesh )


func hide_shock_wave_visual():
	if _shock_wave_visual == null:
		return
	
	_shock_wave_visual.visible = false


# Should return a list of GUI classes to instantiate in a container window which is opened 
# when interation icon is clicked.
# Mode might specify what game mose it is. For example, if it is parts assembling mode, there might be 
# needed different gui panels.
func gui_classes( mode: Array ):
	var classes = []
	
	var sb: Node = get_assembly_raw()
	if sb != null:
		var s_classes = sb.gui_classes( mode )
		for cl in s_classes:
			classes.push_back( cl )
	
	var empty: bool = mode.is_empty()
	if empty:
		#var cam_mode = load( "res://physics/camera_ctrl/gui_elements/gui_camera_mode.tscn" )
		#var cam_this = load( "res://physics/camera_ctrl/gui_elements/gui_control_this.tscn" )
		#classes.push_back( cam_mode )
		#classes.push_back( cam_this )
		var transform_mode = load( "res://physics/camera_ctrl/gui_elements/gui_transform_3.tscn" )
		classes.push_back( transform_mode )
		var finish_editing_mode = load( "res://physics/camera_ctrl/gui_elements/gui_finish_editing_3.tscn" )
		classes.push_back( finish_editing_mode )
		
	return classes

# Defines GUI classes to be shown.
func gui_mode():
	var ret: Array = []
	var sb: Node = get_assembly_raw()
	if sb != null:
		var more: Array = sb.gui_mode()
		for m in more:
			ret.push_back( m )
	return ret


# Returns the root most body.
func root_most_body():
	var sb = get_assembly()
	if sb != null:
		return sb
	return self


func has_player_control():
	var root: RefFrameRoot = get_ref_frame_root()
	var pc = root.player_control
	var ret: bool = (self == pc)
	return ret


func distance( other: RefFrameNode ):
	var se3: Se3Ref = self.relative_to( other )
	var v: Vector3 = se3.r
	var d: float = v.length()
	return d



func _ign_pre_process( delta ):
	process_inner( delta )


# This one to be able to rederive it in derived classes.
# The problem is _process(delta) can't be redefined in derived classes.
func process_inner( _delta ):
	pass




func _ign_physics_pre_process( delta ):
	update_state_from_physics( delta )





# To make it overridable.
func update_state_from_physics( delta ):
	if _physical != null:
		#if _physical.freeze:
		#	return
		var t: Transform3D = _physical.transform
		var v: Vector3   = _physical.linear_velocity
		var w: Vector3   = _physical.angular_velocity
		self.set_t( t )
		self.set_v( v )
		self.set_w( w )
	



# After parent teleports need to update physical state to physical object.
func update_physics_from_state():
	create_physical()
	
	# There might be a body without physical, right?
	# So need to check if it exists even after create_physical() call.
	if _physical != null:
		var t: Transform3D = self.t()
		_physical.transform = t
		var v: Vector3 = self.v()
		_physical.set_linear_velocity( v )
		var w: Vector3 = self.w()
		_physical.set_angular_velocity( w )



func create_visual():
	return _create_visual( VisualScene )


func create_physical():
	return _create_physical( VisualScene )


func _create_visual( Visual: PackedScene ):
	if _visual != null:
		return _visual
	
	if Visual == null:
		return null
	
	var v = Visual.instantiate()
	
	var t: Transform3D = self.t()
	v.transform = t
	
	var root: Node = RootScene.get_visual_layer_near()
	#v.name = RootScene.get_unique_name_for_visuals( v.name )
	root.add_child( v )
	
	# Check if it is a RigidBody.
	# If it is, set mode to kinematic.
	var rigid_body: RigidBody3D = v as RigidBody3D
	if rigid_body != null:
		rigid_body.freeze = true
	
	_visual = v
	
	# For all visual interaction nodes specify self.
	_traverse_interaction_nodes()
	
	# Searching for shock wave visual.
	_traverse_shock_wave_visuals()
	
	# Own ref. frame visualizer
	if Constants.DEBUG and (get_class() != "SurfaceProvider") and false:
		var OwnRf = preload( "res://physics/force_source/own_ref_frame_visualizer.tscn" )
		var rf = OwnRf.instantiate()
		_visual.add_child( rf )



# Spatials to target camera to.
func pivot_tps( _ind: int = 0 ):
	return null

func pivot_fps( _ind: int = 0 ):
	return null


func _create_physical( Physical: PackedScene ):
	if _physical != null:
		return _physical
	
	if Physical == null:
		return null
	
	# Make sure that parent is physics reference frame.
	var parent_rf: RefFrameNode = parent_physics_ref_frame()
	if parent_rf == null:
		return null
	
	var p: RigidBody3D = Physical.instantiate()
	
	#p.visible = false
	
	var t: Transform3D = self.t()
	p.transform = t
	var v: Vector3 = self.v()
	p.linear_velocity = v
	var w: Vector3 = self.w()
	p.angular_velocity = w
	
	parent_rf.add_physics_body( p )
	
	_physical = p
	
	return _physical




func parent_physics_ref_frame():
	var parent_node: Node = get_parent()
	var parent_rf = parent_node as RefFrameNonInertialNode
	if (parent_rf == null) or ( not is_instance_valid(parent_node) ):
		return null
	return parent_node


func remove_physical():
	if _physical == null:
		return
	var valid: bool = is_instance_valid( _physical )
	if not valid:
		_physical = null
	
	_physical.queue_free()
	_physical = null



func _change_parent( p: Node, recursive_call: bool ):
	return
	
	if recursive_call:
		return
	var assembly: Node = get_assembly_raw()
	if (assembly != null) and is_instance_valid( assembly ):
		assembly.change_parent( p )




# Nothing here by default.
func process_user_input_2( input: Dictionary ):
	pass


# It permits or not showing the window with all the little panels.
func show_click_container():
	var sb: Node = get_assembly_raw()
	if sb != null:
		var res: bool = sb.show_click_container()
		return res
	
	return true




func get_mass():
	if _physical != null:
		return _physical.mass
	
	return 1.0


# May need to be overridden in derived classes in the case if 
# _physical is not a rigid body.
func add_force_torque( F: Vector3, P: Vector3 ):
	var valid: bool = is_instance_valid( _physical )
	if not valid:
		_physical = null
		
	if _physical != null:
		var rb: RigidBody3D = _physical as RigidBody3D
		if rb:
			rb.sleeping = false
			rb.apply_central_force( F )
			rb.apply_torque( P )
	
	if force != null:
		force.set_force( false, F, self.r() )


# Body (like a character) might need to know it.
func set_local_up( up: Vector3 ):
	pass




# When being constructed, parts are not supposed to move.
# So it is possible to make dynamic bodies kinematic.
# And when editing is done, one can switch those back to 
# being dynamic.
# These two should be overwritten.
func activate( root_call: bool = true ):
	#if body_state == BodyState.ACTIVE:
	#	return
	body_state = BodyState.ACTIVE

	update_physics_from_state()
	if _physical != null:
		_physical.sleeping = false
	







func deactivate( root_call: bool = true ):
	#if body_state == BodyState.PASSIVE:
	#	return
	body_state = BodyState.PASSIVE
	
	remove_physical()



func set_process_physics( en: bool ):
	if _physical != null:
		_physical.set_physics_process( en )


func _parent_physics_ref_frame():
	# Check if parent is RefFramePhysics
	var parent_node: Node = get_parent()
	var rf: RefFrameNonInertialNode = parent_node as RefFrameNonInertialNode
	return rf





func set_assembly( new_assembly ):
	if (_assembly != null) and ( is_instance_valid(_assembly) ):
		_assembly.remove_sub_body( self )
	_assembly = new_assembly





func get_assembly():
	if _assembly == null:
		_assembly = create_assembly()
	return _assembly


func get_assembly_raw():
	return _assembly



# This one should be overwritten by decendant classes.
func create_assembly():
	return null



func _serialize( data: Dictionary ):
	data["body_state"]         = int(body_state)
	
	# Need to save this in order to restore it correctly.
	if _visual != null:
		data["visual_name"] = _visual.name
	
	return data



# When this thing is called all objects are created.
# So can assume that all saved paths should be valid.
func _deserialize( data: Dictionary ):
#	var ok: bool = super.deserialize( data )
#	if not ok:
#		return false
	var has_name: bool = data.has( "visual_name" )
	if has_name and (_visual != null):
		_visual.name = data["visual_name"]
	
	return true



func _create_octree_mesh():
	var otm: OctreeMeshGd = get_octree_mesh()
	if otm == null:
		otm = OctreeMeshGd.new()
		add_child( otm )
		_octree_mesh = otm



func get_octree_mesh():
	if _octree_mesh != null:
		return _octree_mesh
	
	var qty: int = get_child_count()
	for i in range(qty):
		var n: Node = get_child( i )
		var b: OctreeMeshGd = n as OctreeMeshGd
		if b != null:
			_octree_mesh = b
			return _octree_mesh
	
	return null



static func unique_child_name( n: Node, name: String ):
	var c: Node = n.get_node_or_null( name )
	if c == null:
		return name
	var index: int = 0
	while true:
		var full_name: String = name + "_" + str( index )
		c = n.get_node_or_null( full_name )
		if c == null:
			return full_name
		index += 1



func get_ref_frame_root():
	var rf: RefFrameNode = self
	while rf != null:
		var root: RefFrameRoot = rf as RefFrameRoot
		if root != null:
			return root
		
		rf = rf.get_parent() as RefFrameNode
	
	return null




