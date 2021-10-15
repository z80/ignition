
extends Object

var _noise: OpenSimplexNoise = null



# Planet dimensions.
var height: float = 0.3
var radius: float = 1.0

# Called when the node enters the scene tree for the first time.
func _init():
	init()
	
func init( radius: float = 1.0, height: float = 0.2 ):
	_noise = OpenSimplexNoise.new()
	_noise.period = 1.0
	_noise.octaves = 1


func height( at: Vector3 ):
	var h: float = noise( at )
	return h



func color( at: Vector3, norm: Vector3, height: float ):
	var c: Color
	if height <= 0.0:
		c = Color( 0.1, 0.1, 0.1, 1.0 )
	elif height < 0.3:
		c = Color( 0.4, 0.4, 0.4, 1.0 )
	else:
		c = Color( 0.9, 0.9, 0.9, 1.0 )
	return c



func noise( v: Vector3 ):
	var n: OpenSimplexNoise = _noise
	var mag: Array = [1.0, 0.3, 0.2, 0.1]
	var lac: Array = [5.0, 2.03, 2.07, 2.11]

	var f: float = 0.0
	for i in range(4):
		v *= lac[i]
		var r: float = n.get_noise_3dv( v )
		var t: float = mag[i] * r*r
		f += t
		
	return f
	
	

