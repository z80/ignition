
extends ImmediateGeometry
class_name NodeDrawer

export(Material) var material = null

var _nodes: Array = []


func add_node( node: Array ):
	_nodes.push_back( node )


func clear_nodes():
	_nodes.clear()


# Called when the node enters the scene tree for the first time.
func _ready():
	material_override = material



# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass




func draw():
	clear()
	
	begin( Mesh.PRIMITIVE_LINES )
	
	var qty: int = _nodes.size()
	for i in range(qty):
		var n: Array = _nodes[i]
		_add_node( n )
	
	end()



func _add_node( n: Array ):
	_add_line( n[0], n[1] )
	_add_line( n[1], n[2] )
	_add_line( n[2], n[3] )
	_add_line( n[3], n[0] )

	_add_line( n[0], n[4] )
	_add_line( n[1], n[5] )
	_add_line( n[2], n[6] )
	_add_line( n[3], n[7] )

	_add_line( n[4], n[5] )
	_add_line( n[5], n[6] )
	_add_line( n[6], n[7] )
	_add_line( n[7], n[4] )






func _add_line( a: Vector3, b: Vector3 ):
	add_vertex( a )
	add_vertex( b )



