
extends Node
class_name ForceSource

func ref_frame():
	var p: RefFrame = self.get_parent() as RefFrame
	return p

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


func process_body( phys_rf: RefFramePhysics, body: Body ):
	# Compute relative 
	var force_rf: RefFrame = ref_frame()
	if force_rf == null:
		return
	
	body.compute_relative_to_root( force_rf )
	var r: Vector3 = body.r_root()
	var v: Vector3 = body.v_root()
	var q: Quat    = body.q_root()
	var w: Vector3 = body.w_root()
	
	var ret: Array = []
	compute_force( body, r, v, q, w, ret )
	
	var F: Vector3 = ret[0]
	var P: Vector3 = ret[1]
	# Convert to physics ref. frame.
	var q_adj: Quat = body.q()
	
	F = q_adj.xform( F )
	P = q_adj.xform( P )
	
	# Apply force and torque to the body.
	body.add_force_torque( F, P )
	
	# Befor computing own force contribution recursively searches for 
	# other force sources if recursive search is allowed.
	if recursive():
		var fs: ForceSource = parent_force_source()
		if fs:
			fs.process_body( phys_rf, body )
	

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func parent_force_source():
	var rf: RefFrame = parent_ref_frame()
	var fs: ForceSource = rf.force_source
	return fs


func parent_ref_frame():
	var rf: RefFrame = ref_frame()
	if rf == null:
		return null
	
	var p: Node = rf.parent()
	while true:
		rf = p as RefFrame
		if rf != null:
			return rf
		p = p.get_parent()
		if p == null:
			return null





