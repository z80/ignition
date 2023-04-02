extends Node3D


# Declare member variables here. Examples:
# var a = 2
# var b = "text"

func _init():
	print( "ForceVisualizer::_init(): ", self )
	
func _leave_tree():
	if is_queued_for_deletion():
		print( "ForceVisualizer::on_delete(): ", self )


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func set_force( visible: bool, F: Vector3, r: Vector3 ):
	var l: float = F.length()
	var non_zero: bool = (l > 0.0001)
	self.visible = visible and non_zero
	if self.visible:
		var az: float = atan2( F.y, F.x )
		var el: float = asin( F.z )
		var q1: Quaternion = Quaternion( Vector3(0.0, 0.0, 1.0), az)
		var q2: Quaternion = Quaternion( Vector3(1.0, 0.0, 0.0), el)
		var t: Transform3D = self.transform
		t.basis = q1 * q2
		t.origin = r
		self.transform = t
		
