
extends Object

var _noise: OpenSimplexNoise = null



const POLAR_CAP: float = 0.85

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
	if abs(at.y) > POLAR_CAP:
		c = Color( 0.95, 0.95, 0.95, 1.0 )
		return c
	if height <= 0.0:
		c = Color( 0.2, 0.2, 0.7, 1.0 )
	elif height < 0.2:
		c = Color( 0.0, 0.7, 0.0, 1.0 )
	else:
		c = Color( 0.5, 0.5, 0.2, 1.0 )
	return c



func noise( v: Vector3 ):
	var n: OpenSimplexNoise = _noise
	var f: float = 0.0
	
	if abs(v.y) > POLAR_CAP:
		var a: Vector3 = v
		var mag: Array = [1.0, 0.3, 0.2, 0.1]
		var lac: Array = [ 0.5, 1.03, 1.07, 1.11 ]
		for i in range(4):
			v *= lac[i]
			var t: float = mag[i] * abs( n.get_noise_3dv( v ) )
			f += t
	
	var mag: Array = [1.0, 0.3, 0.2, 0.1]
	var lac: Array = [ 1.0, 2.03, 2.07, 2.11 ]
	for i in range(2):
		v *= lac[i]
		var t: float = mag[i] * n.get_noise_3dv( v )
		f += t
	if f < 0.0:
		f = - sqrt(-f)
	else:
		mag = [1.0, 0.3, 0.2, 0.1]
		lac = [ 5.0, 2.03, 2.07, 2.11 ]
		for i in range(4):
			v *= lac[i]
			var r: float = n.get_noise_3dv( v )
			var t: float = mag[i] * r*r
			f += t
		
	
	
	return f
	




