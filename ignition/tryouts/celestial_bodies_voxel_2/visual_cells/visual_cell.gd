
extends Spatial


var solid: MeshInstance = null
var liquid: MeshInstance = null


# Called when the node enters the scene tree for the first time.
func _ready():
	solid  = get_node( "Solid" )
	liquid = get_node( "Liquid" )



