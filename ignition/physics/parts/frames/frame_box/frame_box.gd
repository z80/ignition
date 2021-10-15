
extends Part


func init():
	hint_text = "Concrete cube. Can be used as a construction element."
	var Visual   = load( "res://physics/frames/frame_box/frame_box_visual.tscn")
	var Physical = load( "res://physics/frames/frame_box/frame_box_physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()



func set_collision_layer( layer ):
	if _physical:
		var rb = _physical
		rb.set_collision_layer( layer )



func create_physical():
	var ph = .create_physical()
	if ph != null:
		ph.mode = ph.MODE_STATIC


