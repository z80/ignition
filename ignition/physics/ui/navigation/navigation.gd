extends Control


enum NavigationMode { SURFACE=0, ORBIT=1, TARGET=2 }
var mode: int = NavigationMode.ORBIT


# Called when the node enters the scene tree for the first time.
func _ready():
	set_mode_orbit()


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
	var camera: RefFrameNode = RootScene.ref_frame_root.player_camera
	if not camera:
		return
	
	var ctrl: PhysicsBodyBase = camera.get_parent() as PhysicsBodyBase
	if ctrl == null:
		return
	var ClosestCelestialBody = preload( "res://physics/utils/closest_celestial_body.gd" )
	var p: Node = ctrl.get_parent()
	var cb: CelestialSurfaceVoxel = ClosestCelestialBody.closest_celestial_body( p ) as CelestialSurfaceVoxel
	if cb == null:
		return
	var rot: RefFrameNode = cb.rotation_rf()
	
	var se3: Se3Ref = ctrl.relative_to( rot )
	var navball = get_node( "SubViewportContainer/SubViewport/Navball" )
	navball.set_orientation_surface( se3 )
	
	# Compute prograde/retrograde.
	var v: Vector3 = se3.v
	var q_inv: Quaternion = se3.q.inverse()
	var _q_i: Quaternion = Quaternion.IDENTITY
	v = q_inv * (v)
	
	var r: Vector3 = se3.r.normalized()
	r = q_inv * (r)
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
	var speed_lbl: Label = get_node( "Speed" )
	speed_lbl.text = "Air speed:  " + str( speed ) + "m/s"
	
	var dist_km: float = se3.r.length()* 0.001 - cb.radius_km
	var dist_lbl: Label = get_node( "GeoidDist" )
	dist_lbl.text  = "Geoid dist: " + str(dist_km) + "km"

	var cs: CelestialSurfaceVoxel = cb as CelestialSurfaceVoxel
	if cs != null:
		var P: float = cs.air_pressure( se3 ) * 0.001
		var air_pressure_lbl: Label = get_node( "AirPressure" )
		air_pressure_lbl.text  = "Air pressure: " + str(P) + "kPa"




func _recompute_mode_orbit():
	var camera: RefFrameNode = RootScene.ref_frame_root.player_camera
	if not is_instance_valid(camera):
		return
	
	var ctrl: PhysicsBodyBase = camera.get_parent() as PhysicsBodyBase
	if ctrl == null:
		return
	var ClosestCelestialBody = preload( "res://physics/utils/closest_celestial_body.gd" )
	var p: Node = ctrl.get_parent()
	var cb: CelestialBody = ClosestCelestialBody.closest_celestial_body( p )
	if cb == null:
		return
	var tran: RefFrameNode = cb
	
	var se3: Se3Ref = ctrl.relative_to( tran )
	
	var cs: CelestialSurfaceVoxel = cb as CelestialSurfaceVoxel
	var se3_rot: Se3Ref
	if cs != null:
		se3_rot = ctrl.relative_to( cs.rotation_rf() )
	
	else:
		se3_rot = se3

	var navball = get_node( "SubViewportContainer/SubViewport/Navball" )
	navball.set_orientation_orbit( se3_rot )
	
	# Compute prograde/retrograde.
	var v: Vector3 = se3.v
	var q_inv: Quaternion = se3.q.inverse()
	#var q_i: Quat = Quat.IDENTITY
	v = q_inv * (v)
	
	var r: Vector3 = se3.r.normalized()
	r = q_inv * (r)
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
	speed_lbl.text = "Speed: " + str( speed ) + "m/s"
	
	var dist_km: float = se3.r.length()* 0.001 - cb.radius_km
	var dist_lbl: Label = get_node( "GeoidDist" )
	dist_lbl.text  = "Geoid dist: " + str(dist_km) + "km"
	
	var P: float
	if cs != null:
		P = cs.air_pressure( se3 ) * 0.001
	
	else:
		P = 0.0
		
	var air_pressure_lbl: Label = get_node( "AirPressure" )
	air_pressure_lbl.text  = "Air pressure: " + str(P) + "kPa"



func _recompute_mode_target():
	var ctrl: PhysicsBodyBase = RootScene.ref_frame_root.player_control as PhysicsBodyBase
	if ctrl == null:
		return
	# TODO: need to implement target first.


# https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
# Only need to convert Godot RF to Wikipedia described ref frame.
func _compute_yaw_pitch_roll( q: Quaternion ):
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
	l.text = "Mode: surface"
	_recompute_mode_surface()


func set_mode_orbit():
	mode = NavigationMode.ORBIT
	var l: Label = get_node( "Mode" )
	l.text = "Mode: orbit"
	_recompute_mode_orbit()


func set_mode_target():
	mode = NavigationMode.TARGET
	var l: Label = get_node( "Mode" )
	l.text = "Mode: target"
	_recompute_mode_target()


func _on_ModeSurface_pressed():
	UiSound.play( Constants.ButtonClick )
	set_mode_surface()


func _on_ModeOrbit_pressed():
	UiSound.play( Constants.ButtonClick )
	set_mode_orbit()


func _on_ModeTarget_pressed():
	UiSound.play( Constants.ButtonClick )
	set_mode_target()


func _on_Sas_pressed():
	UiSound.play( Constants.ButtonClick )
	var check = get_node( "Sas" )
	var down: bool = check.pressed
	UserInput.gui_control_bool( "gui_sas", true, down, not down )







func _on_ShowOrbits_pressed():
	UiSound.play( Constants.ButtonClick )
	var check = get_node( "ShowOrbits" )
	var down: bool = check.pressed
	RootScene.ref_frame_root.visualize_orbits = down



func _on_Map_pressed():
	UiSound.play( Constants.ButtonClick )
	var btn: Button = get_node( "ModeMap" )
	var pressed: bool = btn.pressed
	UserInput.gui_control_bool( "ui_map", pressed, pressed, not pressed )



func _input(event):
	if (event is InputEventKey) and (not event.echo):
		var key_pressed: bool = event.pressed
		#print( "pressed: ", pressed )
		if not key_pressed:
			return
		if event.keycode == KEY_M:
			UiSound.play( Constants.ButtonClick )
			var btn: Button = get_node( "Map" )
			var pressed: bool = btn.pressed
			pressed = not pressed
			btn.button_pressed = pressed
			UserInput.gui_control_bool( "ui_map", pressed, pressed, not pressed )


