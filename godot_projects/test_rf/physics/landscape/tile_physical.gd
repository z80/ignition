extends TileBase


func construct():
	.construct()
	
	var meshes = recompute()
	
	var vertices = meshes[2]
	var concave_sh = ConcavePolygonShape.new()
	concave_sh.set_faces( vertices )
	
	# Create collision.
	var editor_sh = $RigidBody/CollisionShape
	editor_sh.shape = concave_sh
	#$RigidBody.shape_owner_get_shape()
	



func set_collision_bit( bit: int ):
	$RigidBody.collision_layer = bit
	$RigidBody.collision_mask  = bit



