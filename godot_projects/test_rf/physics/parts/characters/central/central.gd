
extends Character


func init():
	VisualType   = load( "res://physics/parts/characters/central/visual.tscn" )
	PhysicalType = load( "res://physics/parts/characters/central/physical.tscn" )
	.init()


func _ready():
	._ready()


