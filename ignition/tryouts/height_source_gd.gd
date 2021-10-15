
extends Object


var noise_rough: OpenSimplexNoise = null


# Called when the node enters the scene tree for the first time.
func _init():
	noise_rough = OpenSimplexNoise.new()
	noise_rough.seed = 0
	noise_rough.octaves = 4
	noise_rough.period = 2.0 * 0.1
	noise_rough.persistence = 0.8
	noise_rough.lacunarity = 0.7


func height( at: Vector3 ):
	var h: float = noise_rough.get_noise_3dv( at )
	return h



func color( at: Vector3, norm: Vector3, height: float ):
	var c: Color
	if height <= 0.0:
		c = Color( 0.7, 0.7, 0.0, 1.0 )
	elif height < 0.1:
		c = Color( 0.0, 0.7, 0.0, 1.0 )
	else:
		c = Color( 0.8, 0.8, 0.8, 1.0 )
	return c
