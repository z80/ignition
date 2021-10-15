
extends Reference
class_name LandscapeHeightFunc

const TOTAL_HEIGHT: float = 5.0

const UNDERSURF_COEF: float = 0.3

const CLIFF_HEIGHT: float = 0.3
const CLIFF_COEF: float = 3.3

const DETAIL_GAIN: float = 0.1
const DETAIL_TH: float = 0.7

var _na: OpenSimplexNoise = OpenSimplexNoise.new()
var _nb: OpenSimplexNoise = OpenSimplexNoise.new()



func _ready():
	_na.octaves = 4
	_na.period = 20.0
	_na.persistence = 0.8

	_nb.octaves = 4
	_nb.period = 5.0
	_nb.persistence = 0.8

func height( x: float, z: float ):
	var h: float = _na.get_noise_2d( x, z )
	if h < 0.0:
		h += 1.0
		h = sqrt( h ) * UNDERSURF_COEF
	elif h > CLIFF_HEIGHT:
		h -= CLIFF_HEIGHT
		h *= CLIFF_COEF
		h *= h
		h += CLIFF_HEIGHT
	
	var d: float = _nb.get_noise_2d( x, z )
	if d >= DETAIL_TH:
		d -= DETAIL_TH
		d *= DETAIL_GAIN
		h += d
	
	h *= TOTAL_HEIGHT
	
	return h
