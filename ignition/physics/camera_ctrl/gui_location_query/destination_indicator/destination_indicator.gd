extends Control

var camera_rf: RefFrameNode = null
var destination_se3: Se3Ref = null

var _btn: TextureButton = null
var _lbl: Label = null


# Called when the node enters the scene tree for the first time.
func _ready():
	_btn = get_node( "Target" )
	_lbl = get_node( "Distance" )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var pos = _relative_pos()
	var vis: bool = (pos != null)
	self.visible = vis
	if not vis:
		return
	
	var dist: float = pos.length()
	var at_xy: Vector2 = _position_on_screen( pos )
	
	_set_distance( dist )
	_set_screen_position( at_xy )





func _relative_pos():
	if (camera_rf == null) or (not is_instance_valid(camera_rf)):
		return null
	
	if (destination_se3 == null):
		return null
	
	var character: RefFrameNode     = camera_rf.get_parent()
	var rf: RefFrameNonInertialNode = character.get_parent()
	var rot: RefFrameRotationNode   = rf.get_parent()

	var rel_se3: Se3Ref = camera_rf.relative_to_se3( rot, destination_se3 )
	rel_se3 = rel_se3.inverse()
	var r: Vector3 = rel_se3.r
	
	return r





func _position_on_screen( rel_r: Vector3 ) -> Vector2:
	var vp: Viewport   = get_viewport()
	var cam: Camera3D  = vp.get_camera_3d()
	var at_3d: Vector3 = rel_r
	if at_3d.z > 0.0:
		return Vector2( 0.0, 0.0 )
	
	var at_2d: Vector2 =  cam.unproject_position( at_3d )
	
	return at_2d



func _set_screen_position( xy: Vector2 ):
	self.position = xy


func _set_distance( dist: float ):
	var stri: String
	dist *= 10.0
	if dist < 1000.0:
		stri = "%3.0f m" % dist
	
	else:
		stri = "%4.2f km" % (dist * 0.001)
	
	_lbl.text = stri






func _input(_event):
	if Input.is_action_just_pressed( "ui_escape" ):
		self.queue_free()




