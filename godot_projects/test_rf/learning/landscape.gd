extends Spatial

var LandscapeTile = preload( "res://learning/landscape_tile.tscn")

export(NodePath) var target_path
onready var target = get_node(target_path) 

var distance: int = 1
var size: float = 5.0
var resolution: int = 4

var _initialized: bool = false
var _point_tile_x: int = 0
var _point_tile_z: int = 0




func set_target( t: Spatial):
	target = t

func get_target():
	return target



# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var needs_recompute: bool = _need_recompute()
	if needs_recompute:
		_recompute_landscape()



func _center():
	var at = target.translation
	var ind_x = int( floor( at.x/size ) )
	var ind_z = int( floor( at.z/size ) )
	return [ind_x, ind_z]


func _need_recompute():
	if not target:
		return false
	if not _initialized:
		return true
	
	var c = _center()
	var ind_x = c[0]
	var ind_z = c[1]
	if (ind_x != _point_tile_x) or (ind_z != _point_tile_x):
		return true
	
	return false


func _recompute_landscape():
	_label_tiles_to_recompute()
	var existing_tiles = _get_existing_tiles()
	print( "\nrebuilding: " )
	for tile in existing_tiles:
		var needs_rebuild: bool = tile.rebuild
		if needs_rebuild:
			print( "(%d, %d)" % [tile.index_x, tile.index_z] )
			tile._construct()
	# Current center to avoid recomputation.
	var c = _center()
	_point_tile_x = c[0]
	_point_tile_x = c[1]
	_initialized = true



func _label_tiles_to_recompute():
	var existing_tiles = _get_existing_tiles()
	
	for tile in existing_tiles:
		tile.rebuild = true
	
	var c = _center()
	var cx = c[0]
	var cz = c[1]
	
	# First pass: 1) mark the ones which no not need to be rebuilt;
	#             2) prepare the list of needed ones.
	var needed_chunks = []
	var n = 2*distance+1
	for iz in range(n):
		var z = iz - distance + cz
		for ix in range(n):
			var x = ix - distance + cx
			
			# Search if it was already computed.
			var found: bool = false
			for tile in existing_tiles:
				var tx: int = tile.index_x
				var tz: int = tile.index_z
				if (tx == x) and (tz == z):
					found = true
					tile.rebuild = false
					break
			if not found:
				needed_chunks.push_back( [x, z] )
	
	
	# Iterate over needed ones and take free ones from the "tile_descs"
	for n in needed_chunks:
		var x = n[0]
		var z = n[1]
		var assigned: bool = false
		for tile in existing_tiles:
			var need_rebuild = tile.rebuild
			if need_rebuild:
				tile.index_x = x
				tile.index_z = z
				assigned = true
				break
		
		# If no spare one found, create one.
		if not assigned:
			var tile = LandscapeTile.instance()
			tile.index_x = x
			tile.index_z = z
			tile.rebuild = true
			self.add_child(tile)
		
	

func _get_existing_tiles():
	var qty = get_child_count()
	var tiles = []
	for i in range(qty):
		var ch = get_child(i)
		var is_tile = ch is MeshInstance
		if is_tile:
			tiles.push_back( ch )
	return tiles




func _inside( at: Vector3, tile ):
	var x = float(tile.index_x) * size
	var z = float(tile.index_z) * size
	if x > at.x:
		return false
	if z > at.z:
		return false
	if at.x > x+size:
		return false
	if at.z > z+size:
		return false
		
	return true



func _dist( at: Vector3, tile ):
	var ix := int( floor( at.x/size ) )
	var iz := int( floor( at.z/size ) )
	var dx: int = tile.index_x - ix
	var dz: int = tile.index_z - iz
	if dx < 0:
		dx = -dx
	if dz < 0:
		dz = -dz
	
	if dx <= dz:
		return dx
	
	return dz


