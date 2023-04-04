extends Control

# This one is for visualizing coupling nodes.
# Or any other 3d objects which are to be drawn on top of everything else.



var _camera: Camera3D = null
var _real_camera: Camera3D = null


func get_subviewport():
	var vp: SubViewport = get_node( "Container/SubViewport" )
	return vp


func _process( _delta: float ):
	if _camera == null:
		_camera = get_node( "Container/SubViewport/Camera3D" )
	if _real_camera == null:
		if (RootScene == null) or (RootScene.ref_frame_root == null):
			return
		var cam_rf: RefFrameNode = RootScene.ref_frame_root.player_camera
		if cam_rf == null:
			return
		_real_camera = cam_rf.get_camera_3d()
	
	var t: Transform3D = _real_camera.transform
	_camera.transform = t



