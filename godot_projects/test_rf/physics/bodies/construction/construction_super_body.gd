
extends Body
class_name ConstructionSuperBody

var construction = null

func _init():
	self.name = "ConstructionSuperBody"


func gui_classes( mode: String = "" ):
	#var in_tree = self.is_inside_tree()
	#self.print_tree_pretty()
	var classes = .gui_classes( mode )
	if mode == "construction":
		var T  = load( "res://physics/bodies/construction/gui_transform_2.tscn" )
		var L  = load( "res://physics/bodies/construction/gui_leave_contruction_mode.tscn" )
		classes.push_back( T )
		classes.push_back( L )
	return classes


func gui_mode():
	if construction == null:
		return ""
	
	if not construction.activated:
		return ""
	
	return "construction"


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



