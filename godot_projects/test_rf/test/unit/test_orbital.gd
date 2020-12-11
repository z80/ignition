
extends "res://addons/gut/test.gd"

var Motion = preload( "res://physics/orbital_motion/motion.gd" )

func before_all():
	pass

func after_all():
	pass


func test_back_forth():
	var gm: float = 1.0
	
	for i in range( 10 ):
		var rx: float  = rand_range( 1.0, 2.0 )
		
		var vx: float  = rand_range( 0.01, 1.0 )
		var vy: float  = rand_range( 0.01, 1.0 )
		var args: Dictionary = Motion.init( gm, Vector3( rx, 0.0, 0.0 ), Vector3( vx, vy, 0.0 ) )
		var ret: Array = Motion.process( 0.0, args )
		print( "r0: ", rx )
		print( "r:  ", ret[0] )
		print( "vx: ", vx, ", vy: ", vy )
		print( "v:  ", ret[1] )
	
	
	
