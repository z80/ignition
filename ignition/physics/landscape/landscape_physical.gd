extends LandscapeBase

var _collision_bit: int = -1


func _ready():
	LandscapeTileType = preload( "res://physics/landscape/tile_physical.tscn" )


func set_collision_bit( bit: int ):
	var tiles = get_existing_tiles()
	_collision_bit = bit
	for t in tiles:
		t.set_collision_bit( _collision_bit )


func setup_tile( tile ):
	.setup_tile( tile )
	if _collision_bit >= 0:
		tile.set_collision_bit( _collision_bit )
