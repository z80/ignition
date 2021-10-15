#tool
extends ImmediateGeometry

const DEBUG: bool = true

export var period: float = 20.0 setget set_period, get_period
export var persistence: float = 0.3 setget set_persistence, get_persistence
export var lacunarity: float = 0.4 setget set_lacunarity, get_lacunarity
export var octaves: int = 4 setget set_octaves, get_octaves
export var size: float = 100.0 setget set_size, get_size
export var resolution: int = 32 setget set_resolution, get_resolution

export var height: float = 1.0 setget set_height, get_height

var _noise = OpenSimplexNoise.new()
var _image: Image = null

var _b_x: int = 0
var _b_y: int = 0

var _color_map = []


# Called when the node enters the scene tree for the first time.
func _ready():
	var c = [ 0.0, Color( 0.0, 0.0, 0.7 ) ]
	_color_map.push_back( c )
	c = [ 0.1, Color( 0.7, 0.7, 0.2 ) ]
	_color_map.push_back( c )

	c = [ 0.2, Color( 0.0, 0.5, 0.0 ) ]
	_color_map.push_back( c )

	c = [ 0.4, Color( 0.1, 0.1, 0.1 ) ]
	_color_map.push_back( c )
	
	c = [ 1.1, Color( 0.8, 0.8, 0.8 ) ]
	_color_map.push_back( c )

	
	_rebuild()


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _block_x():
	var v: Vector3 = translation
	var xf: float = v.x / size
	var xi: int = int( round( xf ) )
	return xi

func _block_y():
	var v: Vector3 = translation
	var yf: float = v.z / size
	var yi: int = int( round( yf ) )
	return yi




func set_period( p: float):
	period = p
	if DEBUG:
		_rebuild()

func get_period():
	return period

func set_persistence( p: float ):
	persistence = p
	if DEBUG:
		_rebuild()

func get_persistence():
	return persistence

func set_lacunarity( l: float):
	lacunarity = l
	if DEBUG:
		_rebuild()
	
func get_lacunarity():
	return lacunarity

func set_octaves( o: int ):
	octaves = o
	if DEBUG:
		_rebuild()
	
func get_octaves():
	return octaves

func set_size( sz: float ):
	size = sz
	if DEBUG:
		_rebuild()

func get_size():
	return size

func set_resolution( res: int ):
	resolution = res
	if DEBUG:
		_rebuild()

func get_resolution():
	return resolution

func set_height( h: float ):
	height = h
	if DEBUG:
		_rebuild()

func get_height():
	return height


func _height_at( ix: int, iy: int ):
	var c: Color = _image.get_pixel( ix, iy )
	var g: float = c.gray()
	var h: float = g * height
	return h


func _color( h: float ):
	for v in _color_map:
		var m: float = v[0]
		if h < m:
			return v[1]
	var sz: int = _color_map.size()
	return _color_map[sz-1][1]


func _rebuild():
	if not _noise:
		return
	if _color_map.empty():
		return
	
	var v: Vector3 = translation
	var bx: int = _block_x()
	var by: int = _block_y()
	var dx: float = v.x - bx*size - size*0.5
	var dy: float = v.z - by*size - size*0.5
	
	_noise.seed = by + 16384*bx
	_noise.period = period
	_noise.persistence = persistence
	_noise.lacunarity  = lacunarity
	_noise.octaves     = octaves
	_image = _noise.get_image( resolution, resolution )
	
	var positions = []
	var normals = []
	var colors = []
	positions.resize( resolution * resolution )
	normals.resize( resolution * resolution )
	colors.resize( resolution * resolution )
	var ind: int = 0
	for iy in range( 0, resolution ):
		for ix in range( 0, resolution ):
			positions[ind] = Vector3.ZERO
			normals[ind]   = Vector3.ZERO
			ind += 1
	
	for iy in range( 1, resolution ):
		var iya: int = iy-1
		var iyb: int = iy
		var ya: float = dy + iya*size/(resolution-1)
		var yb: float = dy + iyb*size/(resolution-1)
		for ix in range( 1, resolution ):
			var ixa: int = ix-1
			var ixb: int = ix
			var xa: float = dx + ixa*size/(resolution-1)
			var xb: float = dx + ixb*size/(resolution-1)
			
			var h0 = _noise.get_noise_2d( xa, ya )
			var h1 = _noise.get_noise_2d( xb, ya )
			var h2 = _noise.get_noise_2d( xb, yb )
			var h3 = _noise.get_noise_2d( xa, yb )
			var c0 = _color( h0 )
			h0 *= height
			h1 *= height
			h2 *= height
			h3 *= height
			var v0 := Vector3( xa, h0, ya )
			var v1 := Vector3( xb, h1, ya )
			var v2 := Vector3( xb, h2, yb )
			var v3 := Vector3( xa, h3, yb )
			var ind0: int = ix + resolution * (iy-1) - 1
			var ind1: int = ind0 + 1
			var ind2: int = ind1 + resolution
			var ind3: int = ind0 + resolution
			
			var va = v3 - v0
			var vb = v1 - v0
			var n: Vector3 = va.cross( vb )
			n = n.normalized()
			normals[ind0] += n
			normals[ind1] += n
			normals[ind3] += n

			va = v1 - v2
			vb = v3 - v2
			n = va.cross( vb )
			n = n.normalized()
			normals[ind1] += n
			normals[ind2] += n
			normals[ind3] += n
			
			positions[ind0] = v0
			positions[ind1] = v1
			positions[ind2] = v2
			positions[ind3] = v3
			
			colors[ind0] = c0
			colors[ind1] = c0
			colors[ind2] = c0
			colors[ind3] = c0
	
	
	ind = 0
	for iy in range( resolution ):
		for ix in range( resolution ):
			var n = normals[ind]
			n = n.normalized()
			normals[ind] = n
			ind += 1
	
	self.clear()
	self.begin( Mesh.PRIMITIVE_TRIANGLES )
	
	for iy in range( 1, resolution ):
		var iya: int = iy-1
		var iyb: int = iy
		for ix in range( 1, resolution ):
			var ixa: int = ix-1
			var ixb: int = ix
			
			var ind0: int = ix + resolution * (iy-1) - 1
			var ind1: int = ind0 + 1
			var ind2: int = ind1 + resolution
			var ind3: int = ind0 + resolution
			
			var v0 = positions[ind0]
			var v1 = positions[ind1]
			var v2 = positions[ind2]
			var v3 = positions[ind3]
			var n0 = normals[ind0]
			var n1 = normals[ind1]
			var n2 = normals[ind2]
			var n3 = normals[ind3]
			var c0 = colors[ind0]
			var c1 = colors[ind1]
			var c2 = colors[ind2]
			var c3 = colors[ind3]

			self.set_normal( n0 )
			self.set_color( c0 )
			self.add_vertex( v0 )
			
			self.set_normal( n1 )
			self.set_color( c0 )
			self.add_vertex( v1 )
			
			self.set_normal( n3 )
			self.set_color( c0 )
			self.add_vertex( v3 )

			
			self.set_normal( n1 )
			self.set_color( c0 )
			self.add_vertex( v1 )

			self.set_normal( n2 )
			self.set_color( c0 )
			self.add_vertex( v2 )

			
			self.set_normal( n3 )
			self.set_color( c0 )
			self.add_vertex( v3 )

	self.end()
	

