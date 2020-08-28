extends Spatial


var capture = null


# Called when the node enters the scene tree for the first time.
func _ready():
	var c: OpenvrCaptureNode = $Capture
	c.init( 2 )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var c: OpenvrCaptureNode = $Capture
	var res: bool = c.process()
	if not res:
		print( "fail" )
	
	var t: Transform = c.pose( 0 )
	# Scale the translation up.
	t.origin = Vector3( t.origin.x, t.origin.y, t.origin.z ) * 10.0
	var m: MeshInstance = $Cube_0
	m.transform = t

	t = c.pose( 1 )
	# Scale the translation up.
	t.origin = Vector3( t.origin.x, t.origin.y, t.origin.z ) * 10.0
	m = $Cube_1
	m.transform = t
