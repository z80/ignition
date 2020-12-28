extends Spatial


var height_source = null


# Called when the node enters the scene tree for the first time.
func _ready():
	var o: RefFrameNode = $Origin
	o.set_r( Vector3( 0.0, 0.0, 20.0 ) )
	
	
	height_source = HeightSourceTestRef.new()
	var sphere: CubeSphereNode = get_node( "sphere" )
	sphere.set_height_source( height_source )
	sphere.radius = 10.0
	sphere.height = 1.0
	
	sphere.add_level( 0.5, 3.0 )
	sphere.add_level( 2.0, 30.0 )
	
	sphere.add_point_of_interest( Vector3( 0.0, 0.0, 10.0 ), true )
	
	var need_to_be_rebuilt: bool = sphere.need_rebuild()
	sphere.rebuild()


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
