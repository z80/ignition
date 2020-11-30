
extends "res://addons/gut/test.gd"

var root: RefFrame = null
var landscape: RefFrame = null
var phys_frame: RefFrame = null
var thruster: RefFrame = null

func before_all():
	var vp: Viewport = get_viewport()
	#root = RefFrame.new()
	#vp.add_child( root )
	#root.set_q( Quat( Vector3( 1.0, 1.0, 1.0 ).normalized(), PI/7.0 ) )
	
	landscape = RefFrame.new()
	vp.add_child( landscape )
	landscape.set_q( Quat( Vector3( 0.0, 1.0, 0.0 ), 0.0 ) )
	
	phys_frame = RefFrame.new()
	landscape.add_child( phys_frame )
	phys_frame.set_q( Quat( Vector3( 0.0, 1.0, 0.0 ), PI/4.0 - 0.1 ) )
	phys_frame.set_r( Vector3( 10.0, 20.0, 30.0 ) )
	
	thruster = RefFrame.new()
	phys_frame.add_child( thruster )
	thruster.set_q( Quat( Vector3( 0.0, 1.0, 0.0 ), PI/4.0 + 0.1 ) )
	thruster.set_r( Vector3( 1.0, 2.0, 3.0 ) )


func after_all():
	if landscape != null:
		landscape.queue_free()
	if root != null:
		root.queue_free()


func test_thruster_relative_to_root():
	#thruster.compute_relative_to_root( root )
	#var qa: Quat = thruster.q_root()
	#root.compute_relative_to_root( thruster )
	#var inv_qa: Quat = root.q_root()
	
	thruster.compute_relative_to_root( landscape )
	var qb: Quat = thruster.q_root()
	landscape.compute_relative_to_root( thruster )
	var inv_qb: Quat = landscape.q_root()
	
	
	
	
	phys_frame.set_jump_q( thruster.q() )
	phys_frame.set_jump_r( thruster.r() )
	phys_frame.apply_jump()

	#thruster.compute_relative_to_root( root )
	#qa = thruster.q_root()
	#root.compute_relative_to_root( thruster )
	#inv_qa = root.q_root()
	
	thruster.compute_relative_to_root( landscape )
	qb = thruster.q_root()
	landscape.compute_relative_to_root( thruster )
	inv_qb = landscape.q_root()

	var qt: Quat = thruster.q()
	var rt: Vector3 = thruster.r()

	
	assert_true( true, "Ba-ba-ba" )



func rand_r( one_axis: bool = false ):
		var x: float = (randf() * 2.0) - 1.0
		var y: float = (randf() * 2.0) - 1.0
		var z: float = (randf() * 2.0) - 1.0
		if one_axis:
			x = 0.0
			y = 0.0
		var v: Vector3 = Vector3( x, y, z )
		v = v.normalized()
		return v

func rand_q( one_axis: bool = false ):
	var v: Vector3 = rand_r( one_axis )
	var a: float = ( (randf() * 2.0) - 1.0 ) * 2.0 * PI
	var q: Quat = Quat( v, a )
	return q


func test_random_ref_frames():
	
	for i in range( 100 ):
		var one_axis: bool = false
		
		var q1: Quat = rand_q( one_axis )
		var r1: Vector3 = rand_r( one_axis )
		phys_frame.set_q( q1 )
		phys_frame.set_r( r1 )
		
		var q2: Quat = rand_q( one_axis )
		var r2: Vector3 = rand_r( one_axis )
		thruster.set_q( q2 )
		thruster.set_r( r2 )
		
		thruster.compute_relative_to_root( landscape )
		var qa: Quat = thruster.q_root()
		landscape.compute_relative_to_root( thruster )
		var inv_qa: Quat = landscape.q_root()
		
		var q_product_a: Quat = q1 * q2
		var q_identity_a: Quat = q_product_a * inv_qa
		var axis: Vector3 = Vector3( q_identity_a.x, q_identity_a.y, q_identity_a.z )
		var l: float = axis.length()
		var ok: bool = (l < 0.0001)
		assert_true( ok, "Fail A: %d" % i )
		if not ok:
			print( "q1: ", q1 )
			print( "q2: ", q2 )
			print( "->", q_product_a )
			print( "<-", q_product_a.inverse() )
			thruster.compute_relative_to_root( landscape )
			landscape.compute_relative_to_root( thruster )
		
		
		phys_frame.set_jump_q( thruster.q() )
		phys_frame.set_jump_r( thruster.r() )
		phys_frame.apply_jump()

		thruster.compute_relative_to_root( landscape )
		var qb: Quat = thruster.q_root()
		landscape.compute_relative_to_root( thruster )
		var inv_qb: Quat = landscape.q_root()
		
		var q_product_b: Quat = q1 * q2
		var q_identity_b: Quat = q_product_b * inv_qb
		axis = Vector3( q_identity_b.x, q_identity_b.y, q_identity_b.z )
		l = axis.length()
		ok = (l < 0.0001)
		assert_true( ok, "Fail B: %d" % i )
		if not ok:
			print( "q1: ", q1 )
			print( "q2: ", q2 )
			print( "->", q_product_a )
			print( "<-", q_product_a.inverse() )
			thruster.compute_relative_to_root( landscape )
			landscape.compute_relative_to_root( thruster )
		
		
		var q3: Quat = rand_q( one_axis )
		var r3: Vector3 = rand_r( one_axis )
		thruster.set_q( q3 )
		thruster.set_r( r3 )

		thruster.compute_relative_to_root( landscape )
		var qc: Quat = thruster.q_root()
		landscape.compute_relative_to_root( thruster )
		var inv_qc: Quat = landscape.q_root()
		
		var q_product_c: Quat = q1 * q2 * q3
		var q_identity_c: Quat = q_product_c * inv_qc
		axis = Vector3( q_identity_c.x, q_identity_c.y, q_identity_c.z )
		l = axis.length()
		ok = (l < 0.0001)
		assert_true( ok, "Fail C: %d" % i )
		if not ok:
			print( "q1: ", q1 )
			print( "q2: ", q2 )
			print( "->", q_product_a )
			print( "<-", q_product_a.inverse() )
			thruster.compute_relative_to_root( landscape )
			landscape.compute_relative_to_root( thruster )




















