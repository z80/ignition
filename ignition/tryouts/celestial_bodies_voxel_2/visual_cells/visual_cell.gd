
extends Spatial


var surface: MeshInstance = null
var liquid: MeshInstance = null


# Called when the node enters the scene tree for the first time.
func _ready():
	surface = get_node( "Surface" )
	liquid  = get_node( "Liquid" )



