extends Control


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
var mm_ = null
var mouse_over_: bool = false
var mouse_pressed_: bool = false

# Called when the node enters the scene tree for the first time.
func _ready():
	get_mm()
	
	var panel = $Panel
	if not panel:
		return
	panel.connect("mouse_entered", self, "_panel_entered")
	panel.connect("mouse_exited", self, "_panel_exited")



# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if not mm_:
		return
		
	var stri: String = "%d" % mm_.frame_ind_
	$Panel/FrameN.text = stri
	
	var q: Quat = mm_.pose_q_;
	var r: Vector3 = mm_.pose_r_
	
	stri = "(%f, %f, %f)" %[ r.x, r.y, r.z ]
	$Panel/PoseR.text = stri

	r = mm_.pose_dr_
	stri = "(%f, %f, %f)" %[ r.x, r.y, r.z ]
	$Panel/PoseDR.text = stri

	stri = "(%f, %f, %f, %f)" %[ q.w, q.x, q.y, q.z ]
	$Panel/PoseQ.text = stri

	q = mm_.pose_dq_;
	stri = "(%f, %f, %f, %f)" %[ q.w, q.x, q.y, q.z ]
	$Panel/PoseDQ.text = stri

	var v2: Vector2 = mm_.control_input_;
	stri = "(%f, %f)" %[ v2.x, v2.y ]
	$Panel/InstantCtrl.text = stri
	
	var all_stri: String = ""
	var qty: int = mm_.print_control_sequence_.size()
	for i in range( qty ):
		var c: Vector2 = mm_.print_control_sequence_[i]
		stri = " (%f, %f)" % [c.x, c.y]
		all_stri = all_stri + stri
	$Panel/InstantCtrl.text = all_stri

func _input(event):
	
	if event is InputEventMouseButton and event.button_index == BUTTON_LEFT:
		mouse_pressed_ = event.pressed
	
	elif event is InputEventMouseMotion and mouse_over_ and mouse_pressed_:
		var panel = $Panel
		var sz = panel.rect_size
		panel.set_begin(panel.get_begin() + event.relative)
		panel.rect_size = sz

func get_mm():
	var p = get_parent()
	mm_ = p


func _panel_entered():
	mouse_over_ = true

func _panel_exited():
	mouse_over_ = false
