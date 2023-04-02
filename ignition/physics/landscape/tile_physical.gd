extends TileBase


func construct():
	super.construct()
	
	var meshes = recompute()
	
	var vertices = meshes[2]
	var concave_sh = ConcavePolygonShape3D.new()
	concave_sh.set_faces( vertices )
	
	# Create collision.
	var editor_sh = $RigidBody3D/CollisionShape3D
	editor_sh.shape = concave_sh
	#$RigidBody.shape_owner_get_shape()
	



func set_collision_bit( bit: int ):
	$RigidBody3D.collision_layer = bit
	$RigidBody3D.collision_mask  = bit



