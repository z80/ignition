extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	var c = $Cube
	var tr = c.transform
	var og = OccupancyGrid.new()
	
	
	#print( og.d_result() )
	#og.d_add( 1 )
	#print( og.d_result() )
	#og.d_add( 2 )
	#print( og.d_result() )
	
	og.set_node_size( 0.1 )
	og.clear()
	og.append( tr, c.mesh )
	og.subdivide()
	
	var pts = og.lines()
	$OccupancyVis.verts = pts


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
