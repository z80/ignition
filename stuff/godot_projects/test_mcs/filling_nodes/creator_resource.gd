
extends Resource

export(PackedScene) var scene = null
export(float) var min_distance = 10.0


func create( parent: Spatial, node: MarchingCubesDualNodeGd, se3: Se3Ref, scaler: DistanceScalerBaseRef ):
	if scene == null:
		return null
	
	var instance: Spatial = scene.instance()
	parent.add_child( instance )
	
	var t: Transform = node.asset_transform( se3, true, true, scaler )
	instance.transform = t
	


func probability( se3_in_source: Se3Ref, norm: Vector3 ):
	return 1.0



