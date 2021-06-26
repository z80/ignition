
extends PartAssembly
class_name ConstructionSuperBody

var construction = null

func _init():
	self.name = "ConstructionSuperBody"




func gui_classes( mode: Array ):
	#var in_tree = self.is_inside_tree()
	#self.print_tree_pretty()
	var classes = .gui_classes( mode )
	if mode.has( "construction_menu" ):
		var T = load( "res://physics/bodies/construction/gui_transform_2.tscn" )
		var D = load( "res://physics/bodies/construction/gui_delete.tscn" )
		var L = load( "res://physics/bodies/construction/gui_leave_contruction_mode.tscn" )
		classes.push_back( T )
		classes.push_back( D )
		classes.push_back( L )
	
	if mode.has( "construction_editing" ):
		var F  = load( "res://physics/bodies/construction/gui_finish_editing.tscn" )
		classes.push_back( F )
		
	return classes


func gui_mode():
	if construction == null:
		return []
	
	if construction.activated_mode == null:
		return []
	
	return [ construction.activated_mode ]


func is_activated():
	if construction == null:
		return false
	
	return construction.activated



func activate_grab( body ):
	if is_instance_valid( construction ):
		construction.activate_grab( body )


func activate_rotate( body ):
	if is_instance_valid( construction ):
		construction.activate_rotate( body )


func finish_editing():
	if is_instance_valid( construction ):
		construction.finish_editing()


