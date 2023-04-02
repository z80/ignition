
extends Resource

@export var scene: PackedScene = null
@export var min_distance: float = 10.0
@export var follow_normal: bool = false

func create( node: MarchingCubesDualNodeGd, source_se3: Se3Ref, se3: Se3Ref, normal: Vector3, rand: IgnRandomGd, scaler: DistanceScalerBaseRef ):
	if scene == null:
		return null
	
	var instance: RefFrameNode = scene.instantiate()
	
	var t: Transform3D = node.asset_transform( source_se3, se3, scaler )

	var q: Quaternion = rand.random_rotation( Vector3.UP, PI, 0.05 )
	t.basis = t.basis * Basis( q )
	
	var scale: float = rand.floating_point_interval_closed( 0.5, 2.0 )
	
	instance.transform = t
	instance.scale     = scale
	
	return instance


func probability( se3_in_source: Se3Ref, norm: Vector3 ):
	return 1.0



