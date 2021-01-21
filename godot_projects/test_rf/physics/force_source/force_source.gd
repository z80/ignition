
extends Reference
class_name ForceSource


func recursive():
	return false

# In order to be able to rotate camera without jumps and 
# weird turns, make force source define vertical.
func defines_vertical():
	return true

# Need to override this one.
# Should use "res://physics/utils/local_up.gd" in order to convert from 
# own rf to player's ref. frame.
func up( own_rf: Node, player_rf: Node ):
	return Vector3.UP


# Compute forces applied to rigid body.
# All quantities are in body's ref. frame.
func compute_force( body: Body, r: Vector3, v: Vector3, q: Quat, w: Vector3, ret: Array ):
	var F: Vector3 = Vector3.ZERO
	var P: Vector3 = Vector3.ZERO
	ret.clear()
	ret.push_back( F )
	ret.push_back( P )







#func parent_force_source():
#	var rf: RefFrame = parent_ref_frame()
#	var fs: ForceSource = rf.force_source
#	return fs
#
#
#func parent_ref_frame():
#	var rf: RefFrame = ref_frame()
#	if rf == null:
#		return null
#
#	var p: Node = rf.parent()
#	while true:
#		rf = p as RefFrame
#		if rf != null:
#			return rf
#		p = p.get_parent()
#		if p == null:
#			return null





