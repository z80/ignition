
extends Resource


@export var height_km: float = 1.0
#export(float) var height_inner_km = 0.2
@export var transparency_dist_outer_km: float = 5.0
@export var transparency_dist_inner_km: float = 30.0
@export var atmosphere_color_day: Color   = Color(0.65, 0.8, 1.0, 1.0)
@export var atmosphere_color_night: Color = Color(1.0, 0.43, 0.46, 1.0)
@export var displacement: float = 0.0

@export var air_density: float          = 1.0
@export var air_viscosity: float        = 0.1
@export var air_pressure_surface: float = 101000.0



# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
