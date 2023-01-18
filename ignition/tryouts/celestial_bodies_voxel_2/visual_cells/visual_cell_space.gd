
extends Spatial


var surface: MeshInstance = null
var liquid: MeshInstance = null


# Called when the node enters the scene tree for the first time.
func _ready():
	var root: Spatial = get_node( "Visual" )
	surface = root.surface
	liquid  = root.liquid



