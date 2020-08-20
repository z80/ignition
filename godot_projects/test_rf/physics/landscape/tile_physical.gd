extends TileBase


func construct():
	# Create collision.
	#var editor_sh = $RigidBody/CollisionShape
	#var sh: Shape = editor_sh.shape
	#sh.set_faces( vertices )
	
	pass


func set_collision_bit( bit: int ):
	$RigidBody.collision_layer = bit
	$RigidBody.collision_mask  = bit



