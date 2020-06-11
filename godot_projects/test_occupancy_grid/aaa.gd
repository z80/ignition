extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	var c = $Cube
	var tr = c.transform
	var og = OccupancyGrid.new()
	print( og.d_result() )
	og.d_add( 1 )
	print( og.d_result() )
	og.d_add( 2 )
	print( og.d_result() )
	
	og.clear()
	og.append( tr, c.mesh )
	og.subdivide()
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
