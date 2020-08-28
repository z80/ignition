extends Spatial


var capture = null


# Called when the node enters the scene tree for the first time.
func _ready():
	var c: OpenvrCaptureNode = $Capture
	c.init( 1 )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var c: OpenvrCaptureNode = $Capture
	var res: bool = c.process()
	if not res:
		print( "fail" )
	
	var t: Transform = c.pose( 0 )
	
	# Scale the translation up.
	t.origin = Vector3( t.origin.x, t.origin.y, t.origin.z ) * 10.0
	var q: Quat = t.basis
	q.y = q.y
	t.basis = q
	
	var m: MeshInstance = $MeshInstance
	m.transform = t
