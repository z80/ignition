
extends PartAssembly
class_name ConstructionSuperBodyNew

var construction = null

func _init():
	self.name = "ConstructionSuperBodyNew"


#func remove_parent():
#	var p = get_parent()
#	if p != null:
#		p.remove_child( self )



func gui_classes( mode: Array ):
	#var in_tree = self.is_inside_tree()
	#self.print_tree_pretty()
	var classes = .gui_classes( mode )
	if mode.has( "construction_menu" ):
		var T = load( "res://physics/bodies/construction_new/gui_elements/gui_transform_2.tscn" )
		var R = load( "res://physics/bodies/construction_new/gui_elements/gui_rotate.tscn" )
		var D = load( "res://physics/bodies/construction_new/gui_elements/gui_delete.tscn" )
		classes.push_back( T )
		classes.push_back( D )
		classes.push_back( R )
	
	if mode.has( "construction_editing" ):
		var F  = load( "res://physics/bodies/construction_new/gui_elements/gui_finish_editing.tscn" )
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


