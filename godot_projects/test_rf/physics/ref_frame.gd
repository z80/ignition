
extends RefFrameNode
class_name RefFrame

var _axes = null

var force_source: Node = null


func process_children():
	pass


func evolve():
	pass




# In this script it is child/parent management
# A separate class is used for physics management and bodies creation.
func _ready():
	ready()


func ready():
	_init_force_source()


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


func _init_force_source():
	if force_source == null:
		for c in self.get_children():
			var fs: ForceSource = c as ForceSource
			if fs != null:
				force_source = fs
				break


# Closest not in terms of distance. But in terms of graph node distance.
func closest_parent_ref_frame():
	var p = self.get_parent()
	if p == null:
		return null
	
	var rf: RefFrame = _closest_parent_ref_frame_recursive( p )
	return rf
	
	
func _closest_parent_ref_frame_recursive( n: Node ):
	if n == null:
		return null
	
	var rf: RefFrame = n as RefFrame
	if rf:
		return rf
	
	var p: Node = n.get_parent()
	return _closest_parent_ref_frame_recursive( p )
	

func closest_force_source():
	var n: Node =  _force_source_recursive( self )
	if n == null:
		return null
	var fs: RefFrame = n as RefFrame
	return fs


func _force_source_recursive( n: Node ):
	if n == null:
		return null
	
	var rf: RefFrame = n as RefFrame
	if rf != null:
		if rf.force_source != null:
			return rf
	
	var p: Node = n.get_parent()
	return _force_source_recursive( p )

