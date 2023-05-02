
extends Resource

@export var scene: PackedScene = null
@export var min_distance: float = 10.0
@export var follow_normal: bool = false

func create( se3: Se3Ref, normal: Vector3, rand: IgnRandomGd ):
	if scene == null:
		return null
	
	var instance: RefFrameNode = scene.instantiate()
	
	var asset_se3: Se3Ref = Se3Ref.new()
	asset_se3.copy_from( se3 )
	var q: Quaternion = rand.random_rotation( Vector3.UP, PI, 0.05 )
	asset_se3.q *= q
	
	var scale: float = rand.floating_point_interval_closed( 0.5, 2.0 )
	
	instance.set_se3( se3 )
	instance.scale = scale
	
	return instance


func probability( se3_in_source: Se3Ref, norm: Vector3 ):
	return 1.0



