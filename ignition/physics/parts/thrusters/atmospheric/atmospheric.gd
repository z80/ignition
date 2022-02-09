
extends Thruster



func init():
	var Visual   = load( "res://physics/parts/thrusters/atmospheric/visual.tscn")
	var Physical = load( "res://physics/parts/thrusters/atmospheric/physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical
	
	sound_start = "res://physics/parts/thrusters/thruster_01/assets/start.ogg"
	sound_loop  = "res://physics/parts/thrusters/thruster_01/assets/loop.ogg"
	sound_stop  = "res://physics/parts/thrusters/thruster_01/assets/stop.ogg"
	sound_failed_start = "res://physics/parts/thrusters/thruster_01/assets/crank_no_start.ogg"

	.init()






