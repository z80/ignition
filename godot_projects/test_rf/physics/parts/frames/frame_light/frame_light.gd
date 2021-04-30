
extends Part


func init():
	hint_text = "Construction light source."
	var Visual   = load( "res://physics/parts/frames/frame_light/frame_light_visual.tscn")
	var Physical = load( "res://physics/parts/frames/frame_light/frame_light_physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()



func set_collision_layer( layer ):
	if _physical:
		var rb = _physical.get_child( 0 )
		rb.set_collision_layer( layer )



func create_physical():
	var ph = .create_physical()
	if ph != null:
		ph.mode = ph.MODE_STATIC


