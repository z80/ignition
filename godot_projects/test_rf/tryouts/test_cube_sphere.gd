extends Spatial


var height_source = null
var distance_scaler = null

# Called when the node enters the scene tree for the first time.
func _ready():
	var o: RefFrameNode = $Origin
	o.set_t( $Camera.transform )

	var sphere: CubeSphereNode = get_node( "sphere" )
	
	distance_scaler = DistanceScalerRef.new()
	distance_scaler.plain_distance = 100.0
	distance_scaler.log_scale      = 0.5
	sphere.distance_scaler = distance_scaler
	
	height_source = HeightSourceTestRef.new()
	sphere.set_height_source( height_source )
	sphere.radius = 10.0
	sphere.height = 1.0
	
	sphere.clear_levels()
	sphere.add_level( 0.5, 3.0 )
	sphere.add_level( 2.0, 30.0 )
	
	


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var o = get_node( "Origin" )
	var c = get_node( "Camera" )
	o.set_t( c.transform )
