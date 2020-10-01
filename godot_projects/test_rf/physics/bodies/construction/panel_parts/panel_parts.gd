extends Control


var construction = null


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Frames_pressed():
	var frames = find_node( "ContainerFrames" )
	var parts = find_node( "ContainerParts" )
	frames.visible = true
	parts.visible  = false
	
	_create_buttons_frames()


func _on_Parts_pressed():
	var frames = find_node( "ContainerFrames" )
	var parts = find_node( "ContainerParts" )
	frames.visible = false
	parts.visible  = true
	
	_create_buttons_parts()


func _cleanup_panel( panel ):
	var children = panel.get_children()
	for ch in children:
		ch.queue_free()


func _add_button( panel, res_icon: String, block_name: String, desc: String, dynamic: bool=true ):
	var Btn = load( "res://physics/bodies/construction/panel_parts/item_button.tscn" )
	var icon = load( res_icon )
	var btn = Btn.instance()
	panel.add_child( btn )
	btn.icon       = icon
	btn.block_name = block_name
	btn.description = desc
	btn.dynamic     = dynamic
	# Connecting signal directly to construction.
	btn.connect( "create_block", construction, "create_block" )


func _create_buttons_frames():
	var frames = find_node( "ContainerFrames" )
	if frames == null:
		return
	_cleanup_panel( frames )
	
	_add_button( frames, "res://physics/bodies/frames/frame_box/icon.png", 
						 "frame_box", 
						 "Frame box description", false )

	_add_button( frames, "res://physics/bodies/frames/frame_light/assets/icon.png", 
						 "frame_light", 
						 "Frame light description", false )


func _create_buttons_parts():
	var parts = find_node( "ContainerParts" )
	if parts == null:
		return
	_cleanup_panel( parts )
	
	_add_button( parts, "res://physics/bodies/parts/part_cylinder/assets/icon.png", 
						 "part_cylinder", 
						 "Cylinder part description", true )












