extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"



# Called when the node enters the scene tree for the first time.
func _ready():
	pass	


func _exit_tree():
	pass


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	var ac = $AnimCtrl
	#  apply_controls( ctrl_v: Vector3, cam_t: Transform = Transform(), tps: bool = true ):
	
	var cam = $Camera
	var cam_t: Transform = cam.global_transform
	
	var is_w: bool = Input.is_action_pressed( "ui_w" )
	var is_s: bool = Input.is_action_pressed( "ui_s" )
	var is_a: bool = Input.is_action_pressed( "ui_a" )
	var is_d: bool = Input.is_action_pressed( "ui_d" )
	var c: Vector3 = Vector3.ZERO
	if is_w:
		c += Vector3.FORWARD
	if is_s:
		c += Vector3.BACK
	if is_a:
		c += Vector3.LEFT
	if is_d:
		c += Vector3.RIGHT
	c = c.normalized()
	ac.apply_controls( c, cam_t, true )


