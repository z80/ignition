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
	parts.visible  = true


func _cleanup_panel( panel ):
	var children = panel.get_children()
	for ch in children:
		ch.queue_free()


func _add_button( panel, res_icon: String, block_name: String, desc: String ):
	var Btn = load( "res://physics/bodies/construction/panel_parts/item_button.tscn" )
	var icon = load( res_icon )
	var btn = Btn.instance()
	panel.add_child( btn )
	btn.icon       = icon
	btn.block_name = block_name
	btn.description = desc
	# Connecting signal directly to construction.
	btn.connect( "create_block", construction, "create_block" )


func _create_buttons_frames():
	var frames = find_node( "ContainerFrames" )
	if frames == null:
		return
	_cleanup_panel( frames )
	
	_add_button( frames, "res://physics/bodies/frames/frame_box/icon.png", 
						 "frame_box", 
						 "Frame box description" )


func _create_buttons_parts():
	var parts = find_node( "ContainerParts" )
	if parts == null:
		return
	_cleanup_panel( parts )












