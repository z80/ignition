extends Exhaust


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Both are assumed to be from 0 to 1.
func set_exhaust( power: float, pressure: float ):
	var mi: MeshInstance = get_node( "inner" )
	var m: Material = mi.get_surface_material( 0 )
	m.set( "shader_param/power", pressure )
	m.set( "shader_param/pressure", pressure )
	
	mi = get_node( "outer" )
	m = mi.get_surface_material( 0 )
	m.set( "shader_param/power", pressure )
	m.set( "shader_param/pressure", pressure )
