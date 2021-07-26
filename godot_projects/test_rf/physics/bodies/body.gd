
extends RefFrameNode
class_name Body

enum BodyState {
	KINEMATIC=0, 
	DYNAMIC=1
}
export(BodyState) var body_state = BodyState.DYNAMIC
# When "load" is called nonpermanent objects are destroyed.
# Permanent objects are not destroyed, they only search for their 
# saved data. Probably, celestial bodies are permanent objects.
export(bool) var is_permanent = false

# Should have an interaction icon onit or not.
# For example, for collision mesh it is not needed.
export(bool) var use_interact_icon = true

# When inheriting need to redefine these two.
var VisualType   = null
var PhysicalType = null

var _visual    = null
var _physical  = null

# Icon showing up when it is relatively close to the camera.
var _icon = null
export(String) var hint_text = "Default hint text" setget _set_hint_text

# Body which contains this one and other bodies.
var super_body = null


# Force visualizer
var force: Spatial = null


func get_class():
	return "Body"





func _enter_tree():
	print( "_enter_tree called on Body" )
	create_physical()



func _exit_tree():
	remove_physical()
	
	var to_be_deleted: bool = is_queued_for_deletion()
	if to_be_deleted:
		on_delete()



func _ready():
	add_to_group( Constants.BODIES_GROUP_NAME )
	
	var Force = preload( "res://physics/force_source/force_visualizer.tscn" )
	force = Force.instance()
	RootScene.get_root_for_visuals().add_child( force )



func _parent_jumped():
	var parent_rf = _parent_physics_ref_frame()
	if parent_rf == null:
		remove_physical()
	else:
		update_physics_from_state()




func init():
	create_visual()



# The overrideable version without "_" prefix.
func on_delete():
	if is_instance_valid( super_body ):
		super_body.remove_sub_body( self )
	if _visual != null:
		_visual.queue_free()
	if _physical != null:
		_physical.queue_free()













func update_visual( root: Node = null ):
	.compute_relative_to_root( root )
	if _visual:
		var t: Transform = self.t_root()
		_visual.transform = t


# Need to overload this in order to apply controls
func update_physical( delta: float ):
	pass


# Should return a list of GUI classes to instantiate in a container window which is opened 
# when interation icon is clicked.
# Mode might specify what game mose it is. For example, if it is parts assembling mode, there might be 
# needed different gui panels.
func gui_classes( mode: Array ):
	var classes = []
	
	if super_body != null:
		var s_classes = super_body.gui_classes( mode )
		for cl in s_classes:
			classes.push_back( cl )
	
	var empty: bool = mode.empty()
	if empty:
		var cam_mode = load( "res://physics/camera_ctrl/gui_elements/gui_camera_mode.tscn" )
		var cam_this = load( "res://physics/camera_ctrl/gui_elements/gui_control_this.tscn" )
		classes.push_back( cam_mode )
		classes.push_back( cam_this )
	return classes

# Defines GUI classes to be shown.
func gui_mode():
	var ret: Array = []
	if super_body != null:
		var more: Array = super_body.gui_mode()
		for m in more:
			ret.push_back( m )
	return ret


# Returns the root most body.
func root_most_body():
	if self.super_body != null:
		return self.super_body
	return self


func has_player_control():
	var pc = PhysicsManager.player_control
	var ret: bool = (self == pc)
	return ret


func distance( other: RefFrameNode ):
	var se3: Se3Ref = self.relative_to( other )
	var v: Vector3 = se3.r
	var d: float = v.length()
	return d



func _process( delta ):
	process_inner( delta )


# This one to be able to rederive it in derived classes.
# The problem is _process(delta) can't be redefined in derived classes.
func process_inner( _delta ):
	_process_interact_icon()




func _physics_process( delta ):
	update_state_from_physics( delta )





# To make it overridable.
func update_state_from_physics( delta ):
	if _physical != null:
		var t: Transform = _physical.transform
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
		var t: Transform = self.t()
		_physical.transform = t
		var v: Vector3 = self.v()
		_physical.set_linear_velocity( v )
		var w: Vector3 = self.w()
		_physical.set_angular_velocity( w )



func create_visual():
	return _create_visual( VisualType )


func create_physical():
	return _create_physical( PhysicalType )


func _create_visual( Visual ):
	if _visual:
		return _visual
	
	if Visual == null:
		return null
	
	var v = Visual.instance()
	
	var t: Transform = self.t()
	v.transform = t
	
	var root = RootScene.get_root_for_visuals()
	root.add_child( v )
	
	_visual = v
	
	# Own ref. frame visualizer
	var OwnRf = preload( "res://physics/force_source/own_ref_frame_visualizer.tscn" )
	var rf = OwnRf.instance()
	_visual.add_child( rf )



# Spatials to target camera to.
func privot_tps( _ind: int = 0 ):
	return _visual

func privot_fps( _ind: int = 0 ):
	return _visual


func _create_physical( Physical ):
	if body_state != BodyState.DYNAMIC:
		return null
	
	if _physical != null:
		return _physical
	
	if Physical == null:
		return null
	
	# Make sure that parent is physics reference frame.
	var parent_rf = parent_physics_ref_frame()
	if parent_rf == null:
		return null
	
	var p = Physical.instance()
	
	var t: Transform = self.t()
	p.transform = t
	var v: Vector3 = self.v()
	p.linear_velocity = v
	var w: Vector3 = self.w()
	p.angular_velocity = w
	
	parent_rf.add_physics_body( p )
	
	_physical = p
	
	return _physical




func parent_physics_ref_frame():
	var parent_node = get_parent()
	var parent_rf = parent_node as RefFrameNode
	if (parent_rf == null) or (parent_rf.get_class() != "RefFramePhysics"):
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



func change_parent( p: Node = null ):
	if (super_body != null) and is_instance_valid( super_body ):
		super_body.change_parent( p )
	else:
		change_parent_inner( p )


func change_parent_inner( p: Node ):
	.change_parent( p )


# Nothing here by default.
func process_user_input_2( input: Dictionary ):
	pass


# It permits or not showing the window with all the little panels.
func show_click_container():
	if super_body != null:
		var res: bool = super_body.show_click_container()
		return res
	
	return true


func _process_interact_icon():
	if not use_interact_icon:
		return
	
	var cam = PhysicsManager.camera
	if (cam == null) or (_visual == null):
		return
	
	var mouse_mode = Input.get_mouse_mode() 
	if (mouse_mode == Input.MOUSE_MODE_HIDDEN) or \
	   (mouse_mode == Input.MOUSE_MODE_CAPTURED):
		if _icon:
			_icon.queue_free()
			_icon = null
		return
	
	var cam_at = cam.translation
	var obj_at = _visual.translation
	var dr = obj_at - cam_at
	var dist = dr.length()
	if dist > Constants.INTERACT_ICON_DIST:
		if _icon:
			_icon.queue_free()
			_icon = null
		return
	
	var vp = get_viewport()
	var rect = vp.get_visible_rect()
	var sz = rect.size
	
	var mouse_at = vp.get_mouse_position()
	var icon_at  =  cam.unproject_position( obj_at )
	var dr2: Vector2 = icon_at - mouse_at
	var height: float = rect.size.y
	var dist2: float = dr2.length() / height
	
	if dist2 > Constants.INTERACT_ICON_SCREEN_DIST:
		if _icon != null:
			_icon.visible = false
			return
	
	if icon_at.x < 0.0 or icon_at.y < 0.0:
		return
	if icon_at.x >= sz.x or icon_at.y >= sz.y:
		return
	
	_create_interact_icon()
	_icon.rect_position = icon_at




func _create_interact_icon():
	if _icon != null:
		_icon.visible = true
		return
	var Icon = load( "res://physics/interact_icon/interact_icon.tscn" )
	_icon = Icon.instance()
	self.add_child( _icon )
	_icon.text = hint_text
	_icon.visible = true


func _remove_interact_icon():
	if _icon == null:
		return
	_icon.queue_free()


func _set_hint_text( stri: String ):
	hint_text = stri
	if _icon == null:
		return
	_icon.text = hint_text



# May need to be overridden in derived classes in the case if 
# _physical is not a rigid body.
func add_force_torque( F: Vector3, P: Vector3 ):
	var valid: bool = is_instance_valid( _physical )
	if not valid:
		_physical = null
		
	if _physical != null:
		var rb: RigidBody = _physical as RigidBody
		if rb:
			rb.add_central_force( F )
			rb.add_torque( P )
			if force != null:
				force.set_force( true, F, self.r() )
	else:
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
	#if body_state == BodyState.DYNAMIC:
	#	return
	body_state = BodyState.DYNAMIC

	update_physics_from_state()
	#_physical.mode = RigidBody.MODE_RIGID
	_physical.sleeping = false
	







func deactivate( root_call: bool = true ):
	#if body_state == BodyState.KINEMATIC:
	#	return
	body_state = BodyState.KINEMATIC
	
	remove_physical()



func set_process_physics( en: bool ):
	if _physical != null:
		_physical.set_physics_process( en )


func _parent_physics_ref_frame():
	# Check if parent is RefFramePhysics
	var parent_node: Node = get_parent()
	var cl_name: String = parent_node.get_class()
	if cl_name != "RefFramePhysics":
		return null
	var parent_rf = parent_node as RefFrameNode
	return parent_rf








func serialize():
	var data: Dictionary = {}
	
	# This is one of the properties.
	var se3: Se3Ref = self.get_se3()
	var se3_data: Dictionary = se3.serialize()
	data["se3"] = se3_data
	
	data["body_state"] = int(body_state)
	
	return data



# When this thing is called all objects are created.
# So can assume that all saved paths should be valid.
func deserialize( data: Dictionary ):
	var se3: Se3Ref = self.get_se3()
	var se3_data: Dictionary = data.se3
	var ret: bool = se3.deserialize( se3_data )
	if not ret:
		return false
	self.set_se3( se3 )
	
	body_state = data["body_state"]
	
#	if restored_body_state == BodyState.DYNAMIC:
#		activate()
#	else:
#		deactivate()
	
	return true



