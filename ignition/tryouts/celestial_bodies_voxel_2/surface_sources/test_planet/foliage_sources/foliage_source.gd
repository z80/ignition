
extends Resource

export(PackedScene) var scene = null
export(float) var min_distance = 10.0
export(bool) var follow_normal = false

func create( node: MarchingCubesDualNodeGd, source_se3: Se3Ref, se3: Se3Ref, normal: Vector3, rand: IgnRandomGd, scaler: DistanceScalerBaseRef ):
	if scene == null:
		return null
	
	var instance: RefFrameNode = scene.instance()
	
	var t: Transform = node.asset_transform( source_se3, se3, scaler )

	var q: Quat = rand.random_rotation( Vector3.UP, PI, 0.05 )
	t.basis = t.basis * Basis( q )
	
	var scale: float = rand.floating_point_interval_closed( 0.5, 2.0 )
	
	instance.transform = t
	instance.scale     = scale
	
	return instance


func probability( se3_in_source: Se3Ref, norm: Vector3 ):
	return 1.0



