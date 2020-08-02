
extends Body


var Visual   = preload( "res://physics/bodies/cube/visual.tscn" )
var Physical = preload( "res://physics/bodies/cube/physical.tscn" )

# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()


func create_visual():
	var v = Visual.instance()
	
	var t: Transform = self.t()
	v.transform = t
	
	get_tree().get_root().add_child( v )
	_physical = v



func create_physical():
	var p = Physical.instance()
	
	var t: Transform = self.t()
	p.transform = t
	
	get_tree().get_root().add_child( p )
	_physical = p
