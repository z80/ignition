
extends RefFrameNode
class_name RefFrame

var _axes = null

var force_source = null



func get_class():
	return RefFrame


func process_children():
	pass


func evolve( _dt: float ):
	pass




# In this script it is child/parent management
# A separate class is used for physics management and bodies creation.
func _ready():
	ready()


func ready():
	init_force_source()


func _process( _delta ):
	_update_axes()




func _create_axes():
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
	
	var t: Transform = self.t_root()
	_axes.transform = t


# Need to be re-defined in subclasses.
func init_force_source():
	force_source = null


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





func _exit_tree():
	var to_be_deleted: bool = is_queued_for_deletion()
	if to_be_deleted:
		on_delete()


func on_delete():
	if _axes != null:
		_axes.queue_free()








func serialize():
	var data: Dictionary = {}
	var se3: Se3Ref = self.get_se3()
	var se3_data: Dictionary = se3.save()
	data.e3 = se3_data
	data




func deserialize( data: Dictionary ):
	var has_se3: bool = data.has["se3"]
	if not has_se3:
		return false
	
	var se3_data: Dictionary = data.se3
	var se3: Se3Ref = self.get_se3()
	se3.load( se3_data )
	self.set_se3( se3 )
	
	return true
	


