extends Node

const ENVS_QTY = 32
var envs_: Array

# All visualization meshes/or other types are with respect to this ref. frame.
var player_ref_frame = null setget _set_player_ref_frame, _get_player_ref_frame
# Ref frame is selected when player clicks the icon.
var player_select = null setget _set_player_select, _get_player_select
# Ref frame gets focus when explicitly pressed "c" (center) on a selected ref. frame.
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
	var player_rf = player_ref_frame
	if player_rf == null:
		return
	
	# Check if need to follow the user object.
	player_rf.evolve()
	player_rf.process_children()
	
	update_bodies_visual()
	update_providers()


func _physics_process( delta ):
	update_bodies_physical( delta )



func _input( event ):
	return
	
	#var body: Body = player_focus as Body
	#if body == null:
	#	return
	
	# Call user input for the upper most body.
	# It is supposed to distribute the same controls 
	# to all sub-bodies.
	#var sb = body.super_body
	#while sb != null:
	#	body = sb
	#	sb = body.super_body
	#body.process_user_input( event )


# Called externally by UserInput global object.
func process_user_input( input: Dictionary ):
	print( "user_input: ", input )
	
	var body: Body = player_focus as Body
	if body == null:
		return
	
	# Call user input for the upper most body.
	# It is supposed to distribute the same controls 
	# to all sub-bodies.
	var sb = body.super_body
	while sb != null:
		body = sb
		sb = body.super_body
	body.process_user_input_2( input )



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



func update_bodies_visual():
	var player_rf = player_ref_frame
	
	# Update visuals for all the physical-visual objects.
	var group: String = Constants.BODIES_GROUP_NAME
	for body in get_tree().get_nodes_in_group( group ):
		body.update_visual( player_rf )



func update_bodies_physical( delta: float ):
	var player_rf = player_ref_frame
	
	# Update visuals for all the physical-visual objects.
	var group: String = Constants.BODIES_GROUP_NAME
	var bodies = get_tree().get_nodes_in_group( group )
	for body in bodies:
		body.update_physical( delta )



func update_providers():
	var group: String = Constants.PROVIDERS_GROUP_NAME
	for pr in get_tree().get_nodes_in_group( group ):
		pr.update()



func _set_player_ref_frame( rf ):
	player_ref_frame = rf


func _get_player_ref_frame():
	return player_ref_frame


func _set_player_select( rf ):
	if is_instance_valid( player_select ):
		var b = player_select as Body
		if b:
			b.process_user_input_2( {} )
	player_select = rf


func _get_player_select():
	return player_select


func _set_player_focus( rf ):
	if is_instance_valid( player_focus ):
		var b = player_focus as Body
		if b:
			b.process_user_input_2( {} )
	player_focus = rf


func _get_player_focus():
	return player_focus


func _set_camera( cam ):
	camera = cam


func _get_camera():
	return camera

