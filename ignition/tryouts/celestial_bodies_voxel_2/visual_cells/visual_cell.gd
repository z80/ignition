
extends Node3D


var solid: MeshInstance3D = null
var liquid: MeshInstance3D = null


# Called when the node enters the scene tree for the first time.
func _ready():
	solid  = get_node( "Solid" )
	liquid = get_node( "Liquid" )



