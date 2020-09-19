tool
extends Node

export(float) var scale=1.0 setget set_scale


func set_scale( s ):
	scale = s

# Called when the node enters the scene tree for the first time.
func _ready():
	$Dn/LeftArm.start(false)
	$Dn/RightArm.start(false)
	$Dn/LeftLeg.start(false)
	$Dn/RightLeg.start(false)
	$Dn/Head.start(false)


func _process(_delta):
	set_frame( null )



func set_frame( f ):
	var trackers = [ $Trackers/Head, 
					$Trackers/LeftArm, 
					$Trackers/RightArm, 
					$Trackers/Torso, 
					$Trackers/LeftLeg, 
					$Trackers/RightLeg ]
	var sz = trackers.size()
	
	if f is Array:
		var transforms = []
		for ind in range( sz ):
			var i = ind*7
			var q: Quat = Quat( f[i+1], f[i+2], f[i+3], f[i] )
			var r: Vector3 = Vector3( f[i+4], f[i+5], f[i+6] ) * scale
			var t: Transform
			t.origin = r
			t.basis  = q
			transforms.push_back( t )
		
		#transforms[3] = transforms[0]
		# Compute relative ones.
		#var gt = transforms[3];
		#var inv_gt = gt.inverse()
		#for ind in range( sz ):
		#	var t = transforms[ind]
		#	t = inv_gt * t
		#	transforms[ind] = t
		
		for ind in range( sz ):
			var t = transforms[ind]
			var tr = trackers[ind]
			tr.transform = t
	
	$Targets/Head.transform = $Trackers/Head/Target.global_transform
	$Targets/LeftArm.transform = $Trackers/LeftArm/Target.global_transform
	$Targets/RightArm.transform = $Trackers/RightArm/Target.global_transform
	$Targets/Torso.transform = $Trackers/Torso/Target.global_transform
	$Targets/LeftLeg.transform = $Trackers/LeftLeg/Target.global_transform
	$Targets/RightLeg.transform = $Trackers/RightLeg/Target.global_transform
	
	$Targets/MagnetHead.transform = $Trackers/Torso/Target/MagnetHead.global_transform
	$Targets/MagnetLeftArm.transform = $Trackers/Torso/Target/MagnetLeftArm.global_transform
	$Targets/MagnetRightArm.transform = $Trackers/Torso/Target/MagnetRightArm.global_transform
	$Targets/MagnetLeftLeg.transform = $Trackers/Torso/Target/MagnetLeftLeg.global_transform
	$Targets/MagnetRightLeg.transform = $Trackers/Torso/Target/MagnetRightLeg.global_transform
	
	var root: Transform = $Targets/Torso.transform
	var inv_root = root.inverse()
	
	var magnet_head: Vector3 = $Targets/MagnetHead.global_transform.origin
	$Dn/Head.magnet = inv_root * magnet_head
	var magnet_left_leg: Vector3 = $Targets/MagnetLeftLeg.global_transform.origin
	$Dn/LeftLeg.magnet = inv_root * magnet_left_leg
	var magnet_right_leg: Vector3 = $Targets/MagnetRightLeg.global_transform.origin
	$Dn/RightLeg.magnet = inv_root * magnet_right_leg
	var magnet_left_arm: Vector3 = $Targets/MagnetLeftArm.global_transform.origin
	$Dn/LeftArm.magnet = inv_root * magnet_left_arm
	var magnet_right_arm: Vector3 = $Targets/MagnetRightArm.global_transform.origin
	$Dn/RightArm.magnet = inv_root * magnet_right_arm
	
	$Dn.transform = $Targets/Torso.global_transform




