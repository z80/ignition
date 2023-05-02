
extends "res://physics/ref_frames/root/ref_frame_root.gd"


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	super._ready()
	UserInput.gui_control_bool( "gui_group_1", true, true, false )
	
#	var cam: Node = get_node( "Planet/Rotation/RefFramePhysics/Character/PlayerCamera" )
#	RootScene.ref_frame_root.player_camera = cam


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
