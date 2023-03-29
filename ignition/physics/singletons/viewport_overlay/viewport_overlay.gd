extends Control

# This one is for visualizing coupling nodes.
# Or any other 3d objects which are to be drawn on top of everything else.



var _camera: Camera = null
var _real_camera: Camera = null


func get_viewport():
	var vp: Viewport = get_node( "Container/Viewport" )
	return vp


func _process( _delta: float ):
	if _camera == null:
		_camera = get_node( "Container/Viewport/Camera" )
	if _real_camera == null:
		if (RootScene == null) or (RootScene.ref_frame_root == null):
			return
		var cam_rf: RefFrameNode = RootScene.ref_frame_root.player_camera
		if cam_rf == null:
			return
		_real_camera = cam_rf.get_camera()
	
	var t: Transform = _real_camera.transform
	_camera.transform = t



