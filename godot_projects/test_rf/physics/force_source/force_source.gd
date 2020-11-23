
extends Node
class_name ForceSource


func recursive():
	return false


# Compute forces applied to rigid body.
# All quantities are in body's ref. frame.
func compute_force( body: Body, r: Vector3, v: Vector3, q: Quat, w: Vector3, ret: Array ):
	var F: Vector3 = Vector3.ZERO
	var P: Vector3 = Vector3.ZERO
	ret.clear()
	ret.push_back( F )
	ret.push_back( P )



	

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


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





