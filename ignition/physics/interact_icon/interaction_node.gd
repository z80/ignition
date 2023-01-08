
extends Spatial
class_name InteractionNode

var target: Node = null setget _set_target, _get_target
var icon_visible: bool = false setget _set_icon_visible, _get_icon_visible

var _icon: Control             = null
var _container_window: Control = null


# Can be overwritten in order to show different icon.
func get_icon_scene():
	return load( "res://physics/interact_icon/icon.tscn" )


# Can be overwritten in order to show different container window.
func get_container_scene():
	return load( "res://physics/interact_icon/container_window.tscn" )



func _set_target( t: Node ):
	target = t


func _get_target():
	return target


# Called when the node enters the scene tree for the first time.
func _ready():
	add_to_group( Constants.INTERACT_NODES_GROUP_NAME )


func _process( delta ):
	if _icon != null:
		var at_2d: Vector2 = _position_on_screen()
		_icon.move_to( at_2d )




func _set_icon_visible( en: bool ):
	# In physical instance of the same scene 
	# target is not specified.
	if (target == null) or (not is_instance_valid(target)):
		return
	
	if icon_visible:
		var under_window: bool = _icon_under_window()
		if under_window:
			_icon.queue_free()
			_icon = null
			icon_visible = false
			return
	
	if icon_visible and (not en):
		if (_icon != null) and is_instance_valid(_icon):
			_icon.queue_free()
			_icon = null
			icon_visible = false
	
	elif (not icon_visible) and en:
		var under_window: bool = _icon_under_window()
		if not under_window:
			_create_icon()
			icon_visible = true



func _get_icon_visible() -> bool:
	return icon_visible



func _create_icon():
	if (_icon != null) and is_instance_valid(_icon):
		return _icon
	
	var Icon: PackedScene = get_icon_scene()
	_icon = Icon.instance()
	var root_node: Node = RootScene.get_root_for_gui_popups()
	root_node.add_child( _icon )
	
	_icon.title = target.name
	
	# In order to position it properly immediately.
	_process(0.0)
	
	_icon.connect( "icon_clicked", self, "_on_clicked" )
	
	return _icon


func distance_to_camera_ray():
	var vp: Viewport = get_viewport()
	var cam: Camera = vp.get_camera()
	if (cam == null) or (not is_instance_valid(cam)):
		return -1.0

	var mouse_mode = Input.get_mouse_mode() 
	if (mouse_mode == Input.MOUSE_MODE_HIDDEN) or \
	   (mouse_mode == Input.MOUSE_MODE_CAPTURED):
		return -1.0
	
	var cam_at: Vector3  = cam.global_transform.origin
	var self_at: Vector3 = self.global_transform.origin
	var dr = self_at - cam_at
	var dist = dr.length()
	if dist > Constants.INTERACT_ICON_DIST:
		return -1.0
	
	var rect: Rect2 = vp.get_visible_rect()
#	var sz: Vector2 = rect.size
	
	var mouse_at: Vector2 = vp.get_mouse_position()
	var icon_at: Vector2  =  cam.unproject_position( self_at )
	var dr2: Vector2 = icon_at - mouse_at
	var height: float = rect.size.y
	var dist2: float = dr2.length() / height
	#print( "dist: ", dist2 )
	
	if dist2 > Constants.INTERACT_ICON_SCREEN_DIST:
		return -1.0
	
	return dist2


func _position_on_screen() -> Vector2:
	var vp: Viewport   = get_viewport()
	var cam: Camera    = vp.get_camera()
	var at_3d: Vector3 = self.global_transform.origin
	var at_2d: Vector2 =  cam.unproject_position( at_3d )
	
	return at_2d

func _icon_under_window() -> bool:
	if _container_window == null:
		return false
	elif not is_instance_valid(_container_window):
		_container_window = null
		return false
	
	var at: Vector2 = _position_on_screen()
	var rect: Rect2 = _container_window.get_global_rect()
	var ret: bool = rect.has_point( at )
	
	return ret


func _on_clicked():
	if (_container_window != null) and (is_instance_valid(_container_window)):
		return
	
	var Window: PackedScene = get_container_scene()
	_container_window = Window.instance()

	var parent_for_windows: Control = RootScene.get_root_for_gui_windows()
	parent_for_windows.add_child( _container_window )
	var at: Vector2 = get_viewport().get_mouse_position()
	_container_window.rect_position = at
	
	_container_window.setup_gui( target )




func _exit_tree():
	if is_queued_for_deletion():
		_on_delete()



func _on_delete():
	if (_container_window != null) and (is_instance_valid(_container_window)):
		_container_window.queue_free()




