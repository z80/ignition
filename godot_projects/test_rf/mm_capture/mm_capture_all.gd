extends Spatial


var capture = null

const QTY: int = 64
const POSITION_SCALE: float = 10.0
var _tracker_inds = [ 1, 2, 5, 6, 7, 8 ]

var _capture: bool = false
var _frames_qty: int = 0

# Called when the node enters the scene tree for the first time.
func _ready():
	_capture = false
	_frames_qty = 0
	
	init_visuals()
	
	var c: OpenvrCaptureNode = $Capture
	c.init( QTY )
	
	$Gui.connect( "start", self, "start_capture" )
	$Gui.connect( "stop",  self, "stop_capture" )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _physics_process(delta):
	var c: OpenvrCaptureNode = $Capture
	var res: bool = c.process()
	if not res:
		print( "fail" )
	
	
	var result = capture_frame()
	var poses = result[0]
	var frame = result[1]
	
	var index: int = 0
	var trackers = $Trackers
	for ind in _tracker_inds:
		var t: Transform = poses[index]
		t.origin *= POSITION_SCALE
		var tr = trackers.get_child( index )
		tr.transform = t
		index += 1
	
	if _capture:
		var db = $FramesDb
		db.set_frame( _frames_qty, frame )
		_frames_qty += 1



func start_capture():
	_capture = true
	_frames_qty = 0
	var db = $FramesDb
	db.set_config( "vive_inds", _tracker_inds )


func stop_capture():
	_capture = false


func capture_frame():
	var c: OpenvrCaptureNode = $Capture
	var frame = []
	var poses = []
	for ind in _tracker_inds:
		var t: Transform = c.pose( ind )
		var r: Vector3 = t.origin
		var q: Quat    = t.basis
		frame.push_back( q.w )
		frame.push_back( q.x )
		frame.push_back( q.y )
		frame.push_back( q.z )
		frame.push_back( r.x )
		frame.push_back( r.y )
		frame.push_back( r.z )
		poses.push_back( t )
	
	return [poses, frame]


func init_visuals():
	var index: int = 0
	var Tr = load( "res://mm_capture/tracker.tscn" )
	var trackers: Node = $Trackers
	for ind in _tracker_inds:
		var tr = Tr.instance()
		tr.name = "Tracker_%d" % index
		trackers.add_child( tr )
		ind += 1


