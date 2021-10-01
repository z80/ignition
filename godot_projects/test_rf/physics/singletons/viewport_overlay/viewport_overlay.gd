extends Control

var _camera: Camera = null
var _real_camera: Camera = null


func get_viewport():
	var vp: Viewport = get_node( "Container/Viewport" )
	return vp


func _process( _delta: float ):
	if _camera == null:
		_camera = get_node( "Container/Viewport/Camera" )
	if _real_camera == null:
		var cam_rf: RefFrameNode = PhysicsManager.camera
		if cam_rf == null:
			return
		_real_camera = cam_rf.get_camera()
	
	var t: Transform = _real_camera.transform
	_camera.transform = t



