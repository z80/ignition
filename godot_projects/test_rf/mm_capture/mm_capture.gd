extends Spatial


var capture = null

var _print_times: int = 10
const QTY: int = 64

# Called when the node enters the scene tree for the first time.
func _ready():
	var c: OpenvrCaptureNode = $Capture
	c.init( QTY )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var c: OpenvrCaptureNode = $Capture
	var res: bool = c.process()
	if not res:
		print( "fail" )
	
	
	if _print_times > 0:
		_print_times -= 1
		var stri: String = ""
		for i in range(QTY):
			var t: Transform = c.pose( i )
			stri += " %f" % t.origin.x
		print( stri )
	
	var t: Transform = c.pose( 5 )
	# Scale the translation up.
	t.origin = Vector3( t.origin.x, t.origin.y, t.origin.z ) * 10.0
	var m: MeshInstance = $Cube_0
	m.transform = t

	t = c.pose( 1 )
	# Scale the translation up.
	t.origin = Vector3( t.origin.x, t.origin.y, t.origin.z ) * 10.0
	m = $Cube_1
	m.transform = t

	t = c.pose( 2 )
	# Scale the translation up.
	t.origin = Vector3( t.origin.x, t.origin.y, t.origin.z + 1.0 ) * 10.0
	m = $Cube_2
	m.transform = t
