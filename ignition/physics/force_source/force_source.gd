
extends Reference
class_name ForceSource




# Compute forces applied to rigid body.
# All quantities are in body's ref. frame.
# "is orbiting" is needed in order to not apply gravity force to a body which is orbiting.
func compute_force( body: PhysicsBodyBase, se3: Se3Ref ):
	var F: Vector3 = Vector3.ZERO
	var P: Vector3 = Vector3.ZERO
	var ret = []
	ret.push_back( F )
	ret.push_back( P )
	return ret






