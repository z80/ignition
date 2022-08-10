tool
extends Resource
class_name SurfaceSourceBase

var source: VolumeSourceGd = null
export(float) var source_dimensions = 0.0 setget , get_source_dimensions

export(Material) var drop_materials_here = null setget _set_drop_materials_here
export(Array) var materials: Array = []

func _set_drop_materials_here( m: Material ):
	drop_materials_here = null
	materials.push_back( m )
	property_list_changed_notify()


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func get_source_dimensions():
	return source_dimensions



