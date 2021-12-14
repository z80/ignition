
extends ReactionControlWheels



func init():
	var Visual   = load( "res://physics/parts/reaction_control_wheels/2m/m1/visual.tscn")
	var Physical = load( "res://physics/parts/reaction_control_wheels/2m/m1/physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	sound = "res://physics/parts/reaction_control_wheels/2m/m1/assets/sound.ogg"

	.init()



