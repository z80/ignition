
extends RefFrameNonInertialNode
class_name RefFrame

var _axes: Spatial = null




func get_class():
	return "RefFrame"


func process_children():
	pass


func evolve( _dt: float ):
	pass




# In this script it is child/parent management
# A separate class is used for physics management and bodies creation.
func _ready():
	ready()


func ready():
	pass


func _process( _delta ):
	_update_axes()




func _create_axes():
	if not Constants.DEBUG:
		return
	
	if _axes != null:
		return false
	
	var root = RootScene.get_root_for_bodies()
	if not root:
		return false
	
	var Axes = preload( "res://physics/axes_visualization.tscn" )
	_axes = Axes.instance()
	root.add_child( _axes )
	
	return true




func _update_axes():
	if not Constants.DEBUG:
		return
	
	if _axes == null:
		var created: bool = _create_axes()
		if not created:
			return
	
#	var t: Transform = self.t_root()
#	_axes.transform = t


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


func get_ref_frame_root():
	var rf: RefFrameNode = RootScene.ref_frame_root
	return rf




func _exit_tree():
	var to_be_deleted: bool = is_queued_for_deletion()
	if to_be_deleted:
		on_delete()


func on_delete():
	if _axes != null:
		_axes.queue_free()










func serialize():
	var data: Dictionary = .serialize()
	return data




func deserialize( data: Dictionary ):
	var ret: bool = .deserialize( data )
	return ret



