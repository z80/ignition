
extends "res://addons/gut/test.gd"

var root: RefFrame = null
var landscape: RefFrame = null
var phys_frame: RefFrame = null
var thruster: RefFrame = null

func before_all():
	var vp: Viewport = get_viewport()
	root = RefFrame.new()
	vp.add_child( root )
	root.set_q( Quat( Vector3( 1.0, 1.0, 1.0 ), PI/7.0 ) )
	
	landscape = RefFrame.new()
	root.add_child( landscape )
	landscape.set_q( Quat( Vector3( 0.0, 1.0, 0.0 ), PI/2.0 ) )
	
	phys_frame = RefFrame.new()
	root.add_child( phys_frame )
	phys_frame.set_q( Quat( Vector3( 0.0, 1.0, 0.0 ), PI/4.0 ) )
	
	thruster = RefFrame.new()
	phys_frame.add_child( thruster )
	thruster.set_q( Quat( Vector3( 0.0, 1.0, 0.0 ), PI/4.0 ) )
	

func after_all():
	root.queue_free()
	
func test_thruster_relative_to_root():
	thruster.compute_relative_to_root( root )
	var qa: Quat = thruster.q_root()
	root.compute_relative_to_root( thruster )
	var inv_qa: Quat = root.q_root()
	
	thruster.compute_relative_to_root( landscape )
	var qb: Quat = thruster.q_root()
	landscape.compute_relative_to_root( thruster )
	var inv_qb: Quat = landscape.q_root()
	
	assert_true( true, "Ba-ba-ba" )

func test_adding_frames():
	assert_true( true, "Ba-ba-ba" )



















