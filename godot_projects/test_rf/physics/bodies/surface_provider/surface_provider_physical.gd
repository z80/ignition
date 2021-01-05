
extends RigidBody

#var transform: Transform setget set_transform, get_transform
#var linear_velocity: Vector3 setget set_linear_velocity, get_linear_velocity
#var angular_velocity: Vector3 setget set_angular_velocity, get_angular_velocity
#var collision_layer: int setget set_collision_layer, get_collision_layer


func set_transform( t: Transform ):
	self.transform = t
	
func get_transform():
	return self.transform

func set_linear_velocity( v: Vector3 ):
	self.linear_velocity = v


func get_linear_velocity():
	return self.linear_velocity


func set_angular_velocity( w: Vector3 ):
	self.angular_velocity = w


func get_angular_velocity():
	return self.angular_velocity


func set_collision_layer( v: int ):
	self.collision_layer = v
	self.collision_mask  = v


func get_collision_layer():
	return self.collision_layer



func set_vertices( vertices: PoolVector3Array ):
	var concave_sh = ConcavePolygonShape.new()
	concave_sh.set_faces( vertices )
	
	# Create collision.
	var editor_sh = get_node( "CollisionShape" )
	editor_sh.shape = concave_sh

