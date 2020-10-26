
extends RigidBody

var body: Body = null

func _init():
	pass


func _physics_process(_delta):
	#var rb: RigidBody = $RigidBody
	#var t: Transform = rb.transform
	#var q: Quat = t.basis.get_rotation_quat()
	#q.x = 0.0
	#q.z = 0.0
	#q = q.normalized()
	#t.basis = q
	#rb.transform = t
	pass





func set_collision_layer( v: int ):
	self.collision_layer = v
	self.collision_mask  = v


func get_collision_layer():
	return self.collision_layer


func apply_force( f: Vector3 ):
	var t: Transform = self.transform
	var q: Quat = t.basis
	var fw: Vector3 = q.xform( f )
	self.add_central_force( fw )

