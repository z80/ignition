
extends Resource


export(float) var height_km = 1.0
#export(float) var height_inner_km = 0.2
export(float) var transparency_dist_outer_km = 5.0
export(float) var transparency_dist_inner_km = 30.0
export(Color) var atmosphere_color_day   = Color(0.65, 0.8, 1.0, 1.0)
export(Color) var atmosphere_color_night = Color(1.0, 0.43, 0.46, 1.0)
export(float) var displacement = 0.0

export(float) var air_density          = 1.0
export(float) var air_viscosity        = 0.1
export(float) var air_pressure_surface = 101000.0



# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
