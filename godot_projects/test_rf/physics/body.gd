
extends RefFrameNode
class_name Body

const GROUP_NAME: String = "bodies"

# When inheriting need to redefine these two.
var VisualType   = null
var PhysicalType = null

var _visual    = null
var _physical  = null

# Icon showing up when it is relatively close to the camera.
var _icon = null
var _hover_text: String = "Default hover text"


func init():
	add_to_group( GROUP_NAME )
	create_visual()



func change_parent( new_parent: Node = null ):
	.change_parent( new_parent )
	update_physical_state_from_rf()



func update_visual( root: Node = null ):
	.compute_relative_to_root( root )
	if _visual:
		var t: Transform = self.t_root()
		_visual.transform = t





func _process( _delta ):
	_process_interact_icon()




func _physics_process( delta ):
	if _physical:
		var t: Transform = _physical.transform
		var v: Vector3   = _physical.linear_velocity
		var w: Vector3   = _physical.angular_velocity
		self.set_t( t )
		self.set_v( v )
		self.set_w( w )


func create_visual():
	return _create_visual( VisualType )


func create_physical():
	return _create_physical( PhysicalType )


func _create_visual( Visual ):
	if _visual:
		return _visual
		
	var v = Visual.instance()
	
	var t: Transform = self.t()
	v.transform = t
	
	var root = BodyCreator.root_node
	root.add_child( v )
	
	_visual = v



func _create_physical( Physical ):
	if _physical:
		return _physical
	
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
	if _physical:
		var t: Transform = self.t()
		_physical.transform = t
		var v: Vector3 = self.v()
		_physical.set_linear_velocity( v )
		var w: Vector3 = self.w()
		_physical.set_angular_velocity( w )



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
	
	var mouse_pos = get_viewport().get_mouse_position()
	var icon_at =  cam.unproject_position( obj_at )
	
	var vp = get_viewport()
	var rect = vp.get_visible_rect()
	var sz = rect.size
	
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
	_icon.body = self
	_icon.text = _hover_text
	_icon.visible = true


func _remove_interact_icon():
	if _icon == null:
		return
	_icon.queue_free()


func _set_hover_text( stri: String ):
	_hover_text = stri
	if _icon == null:
		return
	_icon.text = _hover_text

