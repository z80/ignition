
extends "res://addons/gut/test.gd"

var fs = null

func before_all():
	fs = FrameSearch.new()

func after_all():
	fs = null
	
func test_check_dims():
	var dims_in = 3
	fs.clear()
	fs.set_dims( dims_in )
	var dims_out = fs.dims()
	assert_true( dims_in == dims_out, "Dimentions should match" )

func test_adding_frames():
	fs.clear()
	fs.set_dims( 3 )
	
	var f1 = [-1.0, -1.0, -1.0]
	fs.append( f1 )
	
	f1 = [ 1.0, -1.0, -1.0 ]
	fs.append( f1 )
	
	f1 = [ -1.0, 1.0, -1.0 ]
	fs.append( f1 )

	f1 = [ -1.0, -1.0, 1.0 ]
	fs.append( f1 )
	
	fs.build_tree()
	
	var dist = fs.nearest( [-1.0, -1.0, -1.0] )
	assert_true( dist < 0.001, "Distance from tree node 0 to tree node should be very small" )
	
	var ind = fs.nearest_ind()
	assert_true( ind == 0, "It should be the 0 frame chosen" )


	dist = fs.nearest( [1.0, -1.0, -1.0] )
	assert_true( dist < 0.001, "Distance from tree node 1 to tree node should be very small" )
	
	ind = fs.nearest_ind()
	assert_true( ind == 1, "It should be the 1 frame chosen" )


	dist = fs.nearest( [-1.0, 1.0, -1.0] )
	assert_true( dist < 0.001, "Distance from tree node 2 to tree node should be very small" )
	
	ind = fs.nearest_ind()
	assert_true( ind == 2, "It should be the 2 frame chosen" )



















