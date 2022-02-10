extends Spatial

export(float) var pressure_optimal = 0.8e5
export(float) var pressure_low     = 0.0
export(float) var pressure_high    = 1.0e5
export(float) var thrust = 0.0 setget _set_thrust, _get_thrust
export(float) var pressure = 0.0 setget _set_pressure, _get_pressure




var _carbon_traces: MeshInstance  = null
var _outer_layer: MeshInstance    = null
var _inner_tube: MeshInstance     = null
var _shock_diamonds: MeshInstance = null


func set_exhaust( new_thrust: float, new_pressure: float ):
	_set_thrust( new_thrust )
	_set_pressure( new_pressure )
	


func _set_thrust( v: float):
	thrust = clamp( v, 0.0, 1.0 )
	_update_look()


func _get_thrust():
	return thrust


func _set_pressure( v: float ):
	pressure = v
	_update_look()


func _get_pressure():
	return pressure


func _update_look():
	if not visible:
		return
	
	var clamped_pressure: float = clamp( pressure, pressure_low, pressure_high )

	var ct: MeshInstance = _get_carbon_traces()
	_setup_material( ct, thrust, clamped_pressure )
	
	var ol: MeshInstance = _get_outer_layer()
	var size_y: float = 12.0 * (1.0 + thrust)
	var size: float = size_y
	if clamped_pressure < pressure_optimal:
		#print( "below optimal" )
		var expansion_interval: float = 0.5 * (pressure_optimal - pressure_low)
		var low_point: float = pressure_optimal - expansion_interval
		var scale: float = (clamped_pressure - low_point) / expansion_interval
		#print( "expansion_interval: ", expansion_interval, ", low_point: ", low_point, ", scale: ", scale )
		scale = clamp( scale, 0.3, 1.0 )
		size *= scale
	_setup_material( ol, thrust, clamped_pressure, size )
	
	var it: MeshInstance = _get_inner_tube()
	var sd: MeshInstance = _get_shock_diamonds()
	size = size_y
	if clamped_pressure < pressure_optimal:
		#print( "below optimal" )
		var expansion_interval: float = 0.1 * (pressure_optimal - pressure_low)
		var low_point: float = pressure_optimal - expansion_interval
		var scale: float = (clamped_pressure - low_point) / expansion_interval
		#print( "expansion_interval: ", expansion_interval, ", low_point: ", low_point, ", scale: ", scale )
		scale = clamp( scale, 0.0, 1.0 )
		size *= scale
	#print( "y: ", size )
	_setup_material( it, thrust, clamped_pressure, size )
	_setup_material( sd, thrust, clamped_pressure, size )



func _setup_material( mesh: MeshInstance, thrust: float, pressure_clamped: float, size_y: float = -1.0 ):
	if mesh == null:
		return
	
	mesh.set( "blend_shapes/Thrust", thrust )
	
	var overexpanded: float = (pressure_clamped - pressure_optimal)/(pressure_high - pressure_optimal)
	overexpanded = clamp( overexpanded, 0.0, 1.0 )
	mesh.set( "blend_shapes/Overexpanded", overexpanded )
	
	var underexpanded: float = (pressure_optimal - pressure_clamped)/(pressure_optimal - pressure_low)
	underexpanded = clamp( underexpanded, 0.0, 1.0 )
	mesh.set( "blend_shapes/Underexpanded", underexpanded )
	
	var mat: ShaderMaterial = mesh.get_surface_material( 0 )
	if size_y >= 0.0:
		mat.set_shader_param( "size_y", size_y )
	
	#print( "overexpanded: ", overexpanded, "; underexpanded: ", underexpanded )



func _get_carbon_traces():
	if _carbon_traces == null:
		_carbon_traces = get_node( "CarbonTraces" )
	
	return _carbon_traces


func _get_outer_layer():
	if _outer_layer == null:
		_outer_layer = get_node( "OuterLayer" )
	
	return _outer_layer


func _get_inner_tube():
	if _inner_tube == null:
		_inner_tube = get_node( "InnerTube" )
	
	return _inner_tube


func _get_shock_diamonds():
	if _shock_diamonds == null:
		_shock_diamonds = get_node( "ShockDiamonds" )
	
	return _shock_diamonds



# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
