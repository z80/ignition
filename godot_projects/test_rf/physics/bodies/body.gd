
extends RefFrameNode
class_name Body

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
# List of sub-bodies this body contains if it is a super-body.
var sub_bodies: Array = []


# Force visualizer
var force: Spatial = null

func init():
	create_visual()


func _ready():
	add_to_group( Constants.BODIES_GROUP_NAME )
	
	var Force = preload( "res://physics/force_source/force_visualizer.tscn" )
	force = Force.instance()
	self.add_child( force )



func change_parent( new_parent: Node = null ):
	#var p_before = self.get_parent()
	if super_body != null:
		super_body.change_parent( new_parent )
	else:
		change_parent_recursive( new_parent )
	#var p_after = self.get_parent()



func add_sub_body( body: Body ):
	var sb: Body = body.super_body
	if (sb != null) and (sb != self):
		sb.remove_sub_body( body )
	
	var has: bool = has_sub_body( body )
	if has:
		return false
	
	sub_bodies.push_back( body )
	body.super_body = self
	return true



func remove_sub_body( body: Body ):
	var index: int = sub_bodies.find( body )
	if index >= 0:
		sub_bodies.remove( index )
		body.super_body = null


func is_super_body():
	var empty: bool = sub_bodies.empty()
	var ret: bool = not empty
	return ret


func has_sub_body( body: Body, recursive: bool = true ):
	var has: bool = sub_bodies.has( body )
	if has:
		return true
	
	if recursive:
		for sb in sub_bodies:
			has = sb.has_sub_body( body, true )
			if has:
				return true
	
	return false


func distance_max( other: Body ):
	if other.sub_bodies.empty():
		var d: float = distance( other )
		return d
	
	var max_d: float = -1.0
	for cb in other.sub_bodies:
		var d: float = distance_max( cb )
		if (d > max_d) or (max_d < 0.0):
			max_d = d
	
	return max_d



func distance_min( other: Body ):
	if other.sub_bodies.empty():
		var d: float = distance( other )
		return d
	
	var min_d: float = -1.0
	for cb in other.sub_bodies:
		var d: float = distance_min( cb )
		if (d < min_d) or (min_d < 0.0):
			min_d = d
	
	return min_d



func distance( other: Body ):
	.compute_relative_to_root( other )
	var t: Transform = self.t_root()
	var r: Vector3 = t.origin
	var d: float = r.length()
	return d



func change_parent_recursive( new_parent: Node = null ):
	.change_parent( new_parent )
	
	# First go down to the lowest level.
	for body in sub_bodies:
		body.change_parent_recursive( new_parent )
	
	# After that process corrent level.
	if new_parent == null:
		if _physical != null:
			_physical.queue_free()
			_physical = null
	else:
		var is_ref_frame_physics: bool = new_parent.get_class() == "RefFramePhysics"
		if is_ref_frame_physics:
			if new_parent.is_active():
				update_physical_state_from_rf()
				if _physical:
					_physical.set_collision_layer( new_parent._contact_layer )

		elif _physical != null:
			_physical.queue_free()
			_physical = null


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
func gui_classes( mode: String = "" ):
	var classes = []
	
	if super_body != null:
		var s_classes = super_body.gui_classes( mode )
		for cl in s_classes:
			classes.push_back( cl )
	else:
		if mode.length() < 1:
			var cam_mode = load( "res://physics/camera_ctrl/gui_elements/gui_camera_mode.tscn" )
			var cam_this = load( "res://physics/camera_ctrl/gui_elements/gui_control_this.tscn" )
			classes.push_back( cam_mode )
			classes.push_back( cam_this )
	return classes

# Defines GUI classes to be shown.
func gui_mode():
	if super_body != null:
		return super_body.gui_mode()
	return String("")


# Returns the root most body.
func root_most_body():
	var b = self
	var s = self.super_body
	while s != null:
		b = s
		s = b.super_body
	
	return b



func _process( delta ):
	process_inner( delta )


# This one to be able to rederive it in derived classes.
# The problem is _process(delta) can't be redefined in derived classes.
func process_inner( _delta ):
	_process_interact_icon()




func _physics_process( delta ):
	physics_process_inner( delta )





# To make it overridable.
func physics_process_inner( delta ):
	if _physical:
		var t: Transform = _physical.transform
		var v: Vector3   = _physical.linear_velocity
		var w: Vector3   = _physical.angular_velocity
		self.set_t( t )
		self.set_v( v )
		self.set_w( w )
	



# This one is called later, after all regular bodies got their poses.
func update_super_body_pose():
	for b in sub_bodies:
		var se3: Se3Ref = b.get_se3()
		self.set_se3( se3 )
		break


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
	
	var root = BodyCreator.root_node
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
	if _physical:
		return _physical
	
	if Physical == null:
		return null
	
	var p = Physical.instance()
	
	var t: Transform = self.t()
	p.transform = t
	
	var root = BodyCreator.root_node
	root.add_child( p )
	
	_physical = p
	
	return _physical



func set_collision_layer( layer ):
	if _physical:
		_physical.collision_layer = layer


func remove_physical():
	if _physical != null:
		_physical.queue_free()
		_physical = null


# After parent teleports need to update physical state to physical object.
func update_physical_state_from_rf():
	if _physical == null:
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



# This thing is called in PhysicsManager only for a body which 
# is currently under user control.
# Need to redefine this function in a particular implementation.
func process_user_input( event: InputEvent ):
	for body in sub_bodies:
		body.process_user_input( event )


func process_user_input_2( input: Dictionary ):
	for body in sub_bodies:
		body.process_user_input_2( input )


# It permits or not showing the window with all the little panels.
func show_click_container():
	if super_body != null:
		var res: bool = super_body.show_click_container()
		return res
	
	return true


func _process_interact_icon():
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


# When being constructed podies are not supposed to move.
# So it is possible to make dynamic bodies kinematic.
# And when editing is done, one can switch those back to 
# being dynamic.
# These two should be overwritten.
func activate():
	for body in sub_bodies:
		body.activate()


func deactivate():
	for body in sub_bodies:
		body.deactivate()

# May need to be overridden in derived classes in the case if 
# _physical is not a rigid body.
func add_force_torque( F: Vector3, P: Vector3 ):
	if _physical != null:
		var rb: RigidBody = _physical as RigidBody
		if rb:
			rb.add_central_force( F )
			rb.add_torque( P )
			if force:
				force.set_force( true, F, self.r() )
	else:
		force.set_force( false, F, self.r() )


# Body (like a character) might need to know it.
func set_local_up( up: Vector3 ):
	pass



