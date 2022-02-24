
extends Spatial
class_name ShockWaveVisual

export(Resource) var shock_wave_config = null

var shock_stripes: Array = []
var speed_min: float = 1.0
var speed_max: float = 100.0
var density_min: float = 0.01

# Called when the node enters the scene tree for the first time.
func _ready():
	shock_stripes.push_back( get_node( "ShockStripe_1" ) )
	shock_stripes.push_back( get_node( "ShockStripe_2" ) )
	shock_stripes.push_back( get_node( "ShockStripe_3" ) )
	shock_stripes.push_back( get_node( "ShockStripe_4" ) )
	shock_stripes.push_back( get_node( "ShockStripe_5" ) )
	shock_stripes.push_back( get_node( "ShockStripe_6" ) )
	
	if shock_wave_config != null:
		speed_min   = shock_wave_config.speed_min
		speed_max   = shock_wave_config.speed_max
		density_min = shock_wave_config.density_min



# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

# Velocity need to be relative to air.
func draw_shock_wave( density: float, velocity: Vector3, broad_tree: BroadTreeGd, otree_meshes: Array ):
	var speed: float = velocity.length()
	if (speed < speed_min) or (density < density_min):
		# it is invisible.
		for ss in shock_stripes:
			var shock: Node = ss
			shock.visible = false
	
	else:
		for ss in shock_stripes:
			var shock: Node = ss
			shock.visible = true
			shock.apply( velocity, broad_tree, otree_meshes )
		
		_setup_material( speed )


# Speed relative to air.
func _setup_material( speed: float ):
	
	var air_speed: float   = speed
	var color_speed: float = (speed - speed_min)/(speed_max - speed_min)
	color_speed            = clamp( color_speed, 0.0, 1.0 )
	
	for ss in shock_stripes:
		var shock: Node = ss
		shock.setup_material( air_speed, color_speed )

