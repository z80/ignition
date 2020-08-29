extends Node

const ENVS_QTY = 32
var envs_: Array

# All visualization meshes/or other types are with respect to this ref. frame.
var player_ref_frame = null setget _set_player_ref_frame, _get_player_ref_frame
# Ref frame is selected when player clicks the icon.
var player_select = null setget _set_player_select, _get_player_select
# Ref frame gets focus when eplicitly pressed "c" (center) on a selected ref. frame.
var player_focus = null setget _set_player_focus, _get_player_focus

var camera = null setget _set_camera, _get_camera

# Called when the node enters the scene tree for the first time.
func _ready():
	envs_.resize(ENVS_QTY)
	for i in range(ENVS_QTY):
		envs_[i] = null



# Update body visual parts in accordance with what is set to be 
# player ref. frame. (The ref. frame where the camera is located.)
func _process(_delta):
	var group: String = Body.GROUP_NAME
	var player_rf = player_ref_frame
	for body in get_tree().get_nodes_in_group( group ):
		body.update_visual( player_rf )


func _input( event ):
	var pressed_c: bool = Input.is_action_just_pressed( "ui_c" )
	if pressed_c:
		player_focus = player_select
		camera.privot = player_focus
	
	var body: Body = player_focus as Body
	if body == null:
		return
	body.process_user_input( event )



func acquire_environment( ref_frame ):
	for i in range(ENVS_QTY):
		var e = envs_[i]
		if e == null:
			envs_[i] = ref_frame
			var bit: int = (1 << i)
			return bit
	return -1


func release_environment( ref_frame ):
	for i in range(ENVS_QTY):
		var e = envs_[i]
		if ( e == ref_frame ):
			envs_[i] = null


func available_qty():
	var qty: int = 0
	for i in range(ENVS_QTY):
		var e = envs_[i]
		if e == null:
			qty += 1
			
	return qty


# Is supposed to be used by ref frames in order to generate static collisions 
# in all the ref frames (if close enough).
func physics_ref_frames():
	var envs = {}
	for i in range(ENVS_QTY):
		var e = envs_[i]
		if ( e != null ):
			var bit: int = (1<<i)
			envs[bit] = e
	return envs


func _set_player_ref_frame( rf ):
	player_ref_frame = rf


func _get_player_ref_frame():
	return player_ref_frame


func _set_player_select( rf ):
	player_focus = rf


func _get_player_select():
	return player_focus


func _set_player_focus( rf ):
	player_focus = rf


func _get_player_focus():
	return player_focus


func _set_camera( cam ):
	camera = cam


func _get_camera():
	return camera

