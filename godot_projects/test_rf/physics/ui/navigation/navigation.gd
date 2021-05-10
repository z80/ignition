extends Control


enum NavigationMode { SURFACE=0, ORBIT=1, TARGET=2 }
var mode = NavigationMode.SURFACE


# Called when the node enters the scene tree for the first time.
func _ready():
	set_mode_surface()


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Timer_timeout():
	if mode == NavigationMode.SURFACE:
		_recompute_mode_surface()
	elif mode == NavigationMode.ORBIT:
		_recompute_mode_orbit()
	else:
		_recompute_mode_target()










func _recompute_mode_surface():
	var ctrl: Body = PhysicsManager.player_control as Body
	if ctrl == null:
		return
	var ClosestCelestialBody = preload( "res://physics/utils/closest_celestial_body.gd" )
	var p: Node = ctrl.get_parent()
	var cb: CelestialSurface = ClosestCelestialBody.closest_celestial_body( p )
	if cb == null:
		return
	var rot: RefFrameNode = cb.rotation_rf()
	
	var se3: Se3Ref = ctrl.relative_to( rot )
	var LocalRefFrame = preload( "res://physics/utils/rotation_to.gd" )
	var local_up: Vector3 = se3.r.normalized()
	var q_rf: Quat = LocalRefFrame.rotation_to( Vector3.UP, local_up )
	var q_rel: Quat = q_rf.inverse() * se3.q
	var angles: Array = _compute_yaw_pitch_roll( q_rel )
	var navball = get_node( "ViewportContainer/Viewport/Navball" )
	navball.set_orientation( angles[0], angles[1], angles[2] )
	
	# Compute prograde/retrograde.
	var v: Vector3 = se3.v
	var q_inv: Quat = se3.q.inverse()
	var q_i: Quat = Quat.IDENTITY
	v = q_inv.xform( v )
	
	var r: Vector3 = se3.r.normalized()
	r = q_inv.xform( r )
	var n: Vector3 = r.cross( v )
	var Orthogonalize = preload( "res://physics/utils/orthogonalize.gd" )
	var ret: Array = Orthogonalize.orthogonalize( v, n, r )
	v = ret[0]
	n = ret[1]
	r = ret[2]
	
	navball.set_prograde( v )
	navball.set_retrograde( -v )
	navball.set_normal( n )
	navball.set_anti_normal( -n )
	navball.set_radial_out( r )
	navball.set_radial_in( -r )

	
	# Compute speed relative to surface in the point controlled object is in.
	se3.v = Vector3.ZERO
	se3.w = Vector3.ZERO
	var se3_in_pt: Se3Ref = ctrl.relative_to_se3( rot, se3 )

	var speed: float = se3_in_pt.v.length()
	var speed_lbl = get_node( "Speed" )
	speed_lbl.text = "air speed: " + str( speed ) + "m/s"
	
	



func _recompute_mode_orbit():
	var ctrl: Body = PhysicsManager.player_control as Body
	if ctrl == null:
		return
	var ClosestCelestialBody = preload( "res://physics/utils/closest_celestial_body.gd" )
	var p: Node = ctrl.get_parent()
	var cb: CelestialSurface = ClosestCelestialBody.closest_celestial_body( p )
	if cb == null:
		return
	var tran: RefFrameNode = cb.translation_rf()
	
	var se3: Se3Ref = ctrl.relative_to( tran )
	var LocalRefFrame = preload( "res://physics/utils/rotation_to.gd" )
	var local_up: Vector3 = se3.r.normalized()
	var q_rf: Quat = LocalRefFrame.rotation_to( Vector3.UP, local_up )
	var q_rel: Quat = q_rf.inverse() * se3.q
	var angles: Array = _compute_yaw_pitch_roll( q_rel )
	var navball = get_node( "ViewportContainer/Viewport/Navball" )
	navball.set_orientation( angles[0], angles[1], angles[2] )
	
	# Compute prograde/retrograde.
	var v: Vector3 = se3.v
	var q_inv: Quat = se3.q.inverse()
	var q_i: Quat = Quat.IDENTITY
	v = q_inv.xform( v )
	
	var r: Vector3 = se3.r.normalized()
	r = q_inv.xform( r )
	var n: Vector3 = r.cross( v )
	var Orthogonalize = preload( "res://physics/utils/orthogonalize.gd" )
	var ret: Array = Orthogonalize.orthogonalize( v, n, r )
	v = ret[0]
	n = ret[1]
	r = ret[2]
	
	navball.set_prograde( v )
	navball.set_retrograde( -v )
	navball.set_normal( n )
	navball.set_anti_normal( -n )
	navball.set_radial_out( r )
	navball.set_radial_in( -r )
	
	var speed: float = se3.v.length()
	var speed_lbl = get_node( "Speed" )
	speed_lbl.text = "speed: " + str( speed ) + "m/s"



func _recompute_mode_target():
	var ctrl: Body = PhysicsManager.player_control as Body
	if ctrl == null:
		return
	# For now Use celestial body as a target.
	var ClosestCelestialBody = preload( "res://physics/utils/closest_celestial_body.gd" )
	var p: Node = ctrl.get_parent()
	var cb: CelestialSurface = ClosestCelestialBody.closest_celestial_body( p )
	if cb == null:
		return
	var tran: RefFrameNode = cb.translation_rf()
	
	var se3: Se3Ref = ctrl.relative_to( tran )
	var LocalRefFrame = preload( "res://physics/utils/rotation_to.gd" )
	var local_up: Vector3 = se3.r.normalized()
	var q_rf: Quat = LocalRefFrame.rotation_to( Vector3.UP, local_up )
	var q_rel: Quat = q_rf.inverse() * se3.q
	var angles: Array = _compute_yaw_pitch_roll( q_rel )
	var navball = get_node( "ViewportContainer/Viewport/Navball" )
	navball.set_orientation( angles[0], angles[1], angles[2] )
	
	# Compute prograde/retrograde.
	var r: Vector3 = se3.r
	var q_inv: Quat = se3.q.inverse()
	var q_i: Quat = Quat.IDENTITY
	r = q_inv.xform( r )
	r = r.normalized()
	
	navball.set_prograde( r )
	navball.set_retrograde( -r )
	navball.set_normal( r, false )
	navball.set_anti_normal( r, false )
	navball.set_radial_out( r, false )
	navball.set_radial_in( r, false )
	
	var speed: float = se3.v.length()
	var speed_lbl = get_node( "Speed" )
	speed_lbl.text = "speed: " + str( speed ) + "m/s"



# https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
# Only need to convert Godot RF to Wikipedia described ref frame.
func _compute_yaw_pitch_roll( q: Quat ):
	var qw: float =  q.w
	var qx: float = -q.z
	var qy: float = -q.x
	var qz: float =  q.y
	
	# roll (x-axis rotation)
	var sinr_cosp: float = 2.0 * (qw * qx + qy * qz)
	var cosr_cosp: float = 1.0 - 2.0 * (qx * qx + qy * qy)
	var roll: float = atan2(sinr_cosp, cosr_cosp)
	
	# pitch (y-axis rotation)
	var sinp: float = 2.0 * (qw * qy - qz * qx)
	var pitch: float
	if abs(sinp) >= 1.0:
		# use 90 degrees if out of range
		pitch = PI * 0.5
		if sinp < 0.0:
			pitch = -pitch
	else:
		pitch = asin(sinp)
	
	# yaw (z-axis rotation)
	var siny_cosp: float = 2.0 * (qw * qz + qx * qy)
	var cosy_cosp: float = 1.0 - 2.0 * (qy * qy + qz * qz)
	var yaw: float = atan2(siny_cosp, cosy_cosp);
	
	return [yaw, pitch, roll]


func set_mode_surface():
	mode = NavigationMode.SURFACE
	var l: Label = get_node( "Mode" )
	l.text = "surface"
	_recompute_mode_surface()


func set_mode_orbit():
	mode = NavigationMode.ORBIT
	var l: Label = get_node( "Mode" )
	l.text = "orbit"
	_recompute_mode_orbit()


func set_mode_target():
	mode = NavigationMode.TARGET
	var l: Label = get_node( "Mode" )
	l.text = "target"
	_recompute_mode_target()


func _on_ModeSurface_pressed():
	set_mode_surface()


func _on_ModeOrbit_pressed():
	set_mode_orbit()


func _on_ModeTarget_pressed():
	set_mode_target()


func _on_Sas_pressed():
	var check = get_node( "Sas" )
	var down: bool = check.pressed()
	UserInput.gui_control_bool( "gui_sas", true, down, not down )
