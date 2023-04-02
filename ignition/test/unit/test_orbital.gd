
extends "res://addons/gut/test.gd"

var Motion = preload( "res://physics/orbital_motion/motion.gd" )

func before_all():
	pass

func after_all():
	pass


func test_back_forth_elliptic():
	var gm: float = 1.0
	
	for i in range( 10 ):
		var rx: float  = randf_range( -2.0, 2.0 )
		var ry: float  = randf_range( -2.0, 2.0 )
		
		var vx: float  = randf_range(  -0.11, 0.11 )
		var vy: float  = randf_range(  0.01, 1.0 )
		var args: Dictionary = Motion.init( gm, Vector3( rx, ry, 0.0 ), Vector3( vx, vy, 0.0 ) )
		var ret: Array = Motion.process( 0.0, args )
		print( "rx0: ", rx, ", ry0: ", ry )
		print( "r:  ", ret[0] )
		print( "vx: ", vx, ", vy: ", vy )
		print( "v:  ", ret[1] )
		print( "" )


func test_back_forth_hyperbolic():
	var gm: float = 1.0
	
	for i in range( 10 ):
		var rx: float  = randf_range( -2.0, 2.0 )
		var ry: float  = randf_range( -2.0, 2.0 ) * 0.0
		
		var vx: float  = randf_range(  -0.11, 0.11 )
		var vy: float  = randf_range(  0.01, 1.0 ) + 3.0
		var args: Dictionary = Motion.init( gm, Vector3( rx, ry, 0.0 ), Vector3( vx, vy, 0.0 ) )
		var ret: Array = Motion.process( 0.0, args )
		print( "rx0: ", rx, ", ry0: ", ry )
		print( "r:  ", ret[0] )
		print( "vx: ", vx, ", vy: ", vy )
		print( "v:  ", ret[1] )
		print( "" )



func test_back_forth_parabolic():
	var gm: float = 1.0
	
	for i in range( 10 ):
		var vx: float  = randf_range(  -0.11, 0.11 )
		var vy: float  = randf_range(  0.01, 1.0 )
		
		var v: Vector3 = Vector3( vx, vy, 0.0 )
		var rx: float  = 2.0*gm/v.length_squared()
		#var ry: float  = rand_range( -2.0, 2.0 ) * 0.0
		
		var args: Dictionary = Motion.init( gm, Vector3( rx, 0.0, 0.0 ), Vector3( vx, vy, 0.0 ) )
		var ret: Array = Motion.process( 0.0, args )
		print( "rx0: ", rx )
		print( "r:  ", ret[0] )
		print( "vx: ", vx, ", vy: ", vy )
		print( "v:  ", ret[1] )
		print( "" )

