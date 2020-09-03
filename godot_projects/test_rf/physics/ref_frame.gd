
extends RefFrameNode
class_name RefFrame

var _axes = null



func process_children():
	pass


func evolve():
	pass




# In this script it is child/parent management
# A separate class is used for physics management and bodies creation.
func _ready():
	pass



func _process( _delta ):
	_update_axes()




func _create_axes():
	if _axes:
		return false
	
	var root = BodyCreator.root_node
	if not root:
		return false
	
	var Axes = preload( "res://physics/axes_visualization.tscn" )
	_axes = Axes.instance()
	root.add_child( _axes )
	
	return true




func _update_axes():
	if Constants.DEBUG:
		return
	
	if not _axes:
		var created: bool = _create_axes()
		if not created:
			return
	
	var t: Transform = self.t_root()
	_axes.transform = t







