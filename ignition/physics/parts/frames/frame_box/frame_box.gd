
extends StaticPhysicsBody


func init():
	hint_text = "Concrete cube. Can be used as a construction element."
	var Visual   = load( "res://physics/parts/frames/frame_box/frame_box_visual.tscn")
	var Physical = load( "res://physics/parts/frames/frame_box/frame_box_physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()





func create_physical():
	var ph = .create_physical()
	if ph != null:
		ph.mode = ph.MODE_STATIC


