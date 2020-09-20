
extends Body


func init():
	var Visual   = load( "res://physics/bodies/frames/frame_box/frame_box_visual.tscn")
	var Physical = load( "res://physics/bodies/frames/frame_box/frame_box_physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()



func set_collision_layer( layer ):
	if _physical:
		var rb = _physical.get_child( 0 )
		rb.collision_layer = layer
		rb.collision_mask  = layer



func create_physical():
	var ph = .create_physical()
	if ph != null:
		ph.mode = ph.MODE_STATIC


