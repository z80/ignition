extends Camera

## Path to the glow border effect renderer for update of camera parameters
## and transforms
export var glow_border_effect_renderer_path : NodePath

# Internal cached reference to the glow border effect renderer
onready var _glow_border_effect_renderer = get_node(glow_border_effect_renderer_path)


# Called when the node enters the scene tree for the first time.
func _ready():
	# Update the internal cameras in the glow border effect renderer
	_glow_border_effect_renderer.set_camera_parameters(self)
	
	# Turn on notification for camera transform changes
	set_notify_transform(true)


func _notification(what):
	# Update the camera transform each time the camera transform change
	if what == NOTIFICATION_TRANSFORM_CHANGED:
		_glow_border_effect_renderer.camera_transform_changed(self)
