extends Spatial


var capture = {
	f = null
}



# Called when the node enters the scene tree for the first time.
func _ready():
	var c: OpenvrCaptureNode = $Capture
	c.init( 1 )
	
	$Gui.connect( "start", self, "start_capture" )
	$Gui.connect( "stop",  self, "stop_capture" )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var c: OpenvrCaptureNode = $Capture
	var res: bool = c.process()
	if not res:
		print( "fail" )
	
	var t: Transform = c.pose( 0 )
	
	# Capture to a file if enabled.
	if capture.f != null:
		var ts: String = c.timestamp()
		capture_pose( ts, t )
	
	# Scale the translation up.
	t.origin = Vector3( t.origin.x, t.origin.y, t.origin.z ) * 10.0
	var m: MeshInstance = $Cube_0
	m.transform = t
	


func _terminate():
	stop_capture()



func start_capture():
	stop_capture()
	
	var f: File = File.new()
	f.open( "./vive_pose.log", File.WRITE )
	
	capture.f = f

func stop_capture():
	var f: File = capture.f
	if f == null:
		return
	f.close()


func capture_pose( ts, t ):
	if capture.f == null:
		return
	
	var r: Vector3 = t.origin
	var q: Quat    = t.basis
	var stri = "%s %f %f %f %f %f %f %f\n" % [ts, r.x, r.y, r.z, q.x, q.y, q.z, q.w]
	capture.f.store_string( stri )



