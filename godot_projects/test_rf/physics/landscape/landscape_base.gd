extends Spatial
class_name LandscapeBase

var LandscapeTileType = null

export var distance: int = 1
export var size: float = 5.0
export var resolution: int = 17

var _initialized: bool = false
var _point_tile_x: int = 0
var _point_tile_z: int = 0
# Current relative ref frame transform.
# Tile are places inside of this ref frame.
# So need to undo this transform.
var _rel_t: Transform
# Transfrom for which tiles were computed.
var _applied_rel_t: Transform

var height_func = LandscapeHeightFunc.new()




func height( x: float, z: float ):
	return height_func.height( x, z )



# Called externally when needed.
func update( t: Transform ):
	_rel_t = t
	var needs_recompute: bool = _need_recompute()
	if needs_recompute:
		_recompute_landscape()
	var needs_move: bool = _need_move()
	if needs_move:
		_move_landscape()
	_applied_rel_t = _rel_t



func _center():
	var at: Vector3 = _rel_t.origin
	var ind_x = int( floor( at.x/size ) )
	var ind_z = int( floor( at.z/size ) )
	return [ind_x, ind_z]


func _need_recompute():
	if not _initialized:
		return true
	if _rel_t == _applied_rel_t:
		return false
	
	var c = _center()
	var ind_x = c[0]
	var ind_z = c[1]
	if (ind_x != _point_tile_x) or (ind_z != _point_tile_z):
		return true
	
	return false

func _need_move():
	var res: bool = (_rel_t != _applied_rel_t)
	return res


func _recompute_landscape():
	_label_tiles_to_recompute()
	var existing_tiles = get_existing_tiles()
	print( "\nrebuilding: " )
	for tile in existing_tiles:
		var needs_rebuild: bool = tile.rebuild
		if needs_rebuild:
			print( "(%d, %d)" % [tile.index_x, tile.index_z] )
			tile.construct()
	# Current center to avoid recomputation.
	var c = _center()
	_point_tile_x = c[0]
	_point_tile_z = c[1]
	_initialized = true


func _move_landscape():
	var inv_t = _rel_t.inverse()
	var existing_tiles = get_existing_tiles()
	for tile in existing_tiles:
		var t = tile.transform
		t = inv_t * t
		tile.transform = t
	
	print( "Moved landscape to ", inv_t )


func _label_tiles_to_recompute():
	var existing_tiles = get_existing_tiles()
	
	for tile in existing_tiles:
		tile.rebuild = true
		tile.available = true
	
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
					tile.available = false
					break
			if not found:
				needed_chunks.push_back( [x, z] )
	
	
	# Iterate over needed ones and take free ones from the "tile_descs"
	for nc in needed_chunks:
		var x = nc[0]
		var z = nc[1]
		var assigned: bool = false
		for tile in existing_tiles:
			var available = tile.available
			if available:
				tile.index_x = x
				tile.index_z = z
				tile.available = false
				assigned = true
				break
		
		# If no spare one found, create one.
		if not assigned:
			var tile = LandscapeTileType.instance()
			tile.index_x = x
			tile.index_z = z
			tile.rebuild = true
			self.add_child(tile)
			setup_tile( tile )
		
	

func get_existing_tiles():
	var qty = get_child_count()
	var tiles = []
	for i in range(qty):
		var ch = get_child(i)
		var is_tile = ch is MeshInstance
		if is_tile:
			tiles.push_back( ch )
	return tiles



# Need to redefine this one.
# For collsion object it should set collision mask and layer.
func setup_tile( tile ):
	pass


