extends Control


enum NavigationMode { Surface=0, Orbit=1 }



# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Timer_timeout():
	recompute_surface()










func recompute_surface():
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



func _on_ModeSurface_pressed():
	pass # Replace with function body.


func _on_ModeOrbit_pressed():
	pass # Replace with function body.
