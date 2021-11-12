
extends Reference
class_name CouplingConnection

# Which paret it belongs to.
var part: RefFrameNode = null

# If this part is attached to another one 
# here it is stored "parent" part path.
export(NodePath) var connection_b_path = null
# The other s node is is connected to.
var connection_b: CouplingNode = null
# If it was connected to, it's parent, else it's child.
var is_parent: bool = false

var relative_to_owner: Transform = Transform.IDENTITY

# This one attacheds to the surface.
# When it happens, some default transform is computed. 
# This transform is this one.
var base_transform: Transform = Transform.IDENTITY
# And this transform is the result of "base_transform" plus rotation_angles.
var transform: Transform = Transform.IDENTITY
# If this one is attached to other part 
# need to remember the rotation angle.
export(float) var yaw: float = 0.0 setget _set_yaw, _get_yaw
export(float) var pitch: float = 0.0 setget _set_pitch, _get_pitch
export(float) var roll: float = 0.0 setget _set_roll, _get_roll


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
