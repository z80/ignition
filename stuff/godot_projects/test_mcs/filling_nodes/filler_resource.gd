
extends Node

export(PackedScene) var scene = null
export(float) var min_distance = 10.0


func create( parent: Spatial, node: MarchingCubesDualNodeGd, se3: Se3Ref, scaler: DistanceScalerBaseRef ):
	if scene == null:
		return null
	
	var m: Spatial = scene.instance()
	parent.add_child( m )
	
	var t: Transform = node.asset_transform( se3, false, true, scaler )


func probability( se3: Se3Ref ):
	return 1.0



