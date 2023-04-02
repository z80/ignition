extends Exhaust


@export var pressure_optimal: float = 0.8e5
@export var pressure_low: float     = 0.0
@export var pressure_high: float    = 1.0e5



# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Both are assumed to be from 0 to 1.
func set_exhaust( power: float, pressure: float ):
	var mi: MeshInstance3D = get_node( "inner" )
	var m: Material = mi.get_surface_override_material( 0 )
	m.set( "shader_param/power", pressure )
	m.set( "shader_param/pressure", pressure )
	
	mi = get_node( "outer" )
	m = mi.get_surface_override_material( 0 )
	m.set( "shader_param/power", pressure )
	m.set( "shader_param/pressure", pressure )
