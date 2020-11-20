
extends Node
class_name ForceSource

func ref_frame():
	var p: RefFrame = self.get_parent() as RefFrame
	return p

func recursive():
	return false


# Compute forces applied to rigid body.

func compute_force( body: Body, r: Vector3, v: Vector3, q: Quat, w: Vector3,  ret: Array ):
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
	
	# Befor computing own force contribution recursively searches for 
	# other force sources if recursive search is allowed.
	if recursive():
		# Do be implemented...
		pass
	
	body.compute_relative_to_root( force_rf )
	var r: Vector3 = body.r_root()
	var v: Vector3 = body.v_root()
	var q: Quat    = body.q_root()
	var w: Vector3 = body.w_root()
	phys_rf.compute_relative_to_root( force_rf )
	var q_phys: Quat = phys_rf.q_root()
	var q_adj: Quat = q_phys.inverse()
	
	r = q_adj.xform( r )
	v = q_adj.xform( v )
	q = q_adj * q
	w = q_adj.xform( w )
	
	var ret: Array = []
	compute_force( body, r, v, q, w, ret )
	
	# Apply force and torque to the body.
	# To be implemented.
	# ......
	

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.
	

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
