
extends Spatial
class_name RfBase

# In this script it is child/parent management
# A separate class is used for physics management and bodies creation.

var _ref_frame      = null
var _ref_frame_tree = null

# Called when the node enters the scene tree for the first time.
func _ready():
	_ref_frame_tree.add( self )

func cleanup():
	_ref_frame_tree.remove( self )


func _process(delta):
	if _ref_frame == null:
		return
		
	_ref_frame.compute_relative_to_root()
	var t: Transform = _ref_frame.t_root()
	set_transform( t )


func set_origin( rf ):
	if _ref_frame == null:
		return

	_ref_frame.set_origin( rf )


func set_root( rf ):
	if _ref_frame == null:
		return

	_ref_frame.set_root( rf )


func set_rf_transform( t ):
	if _ref_frame == null:
		return

	_ref_frame.set_rf_transform( t )





