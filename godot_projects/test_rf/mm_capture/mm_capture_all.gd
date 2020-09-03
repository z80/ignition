extends Spatial


var capture = null

const QTY: int = 64
const POSITION_SCALE: float = 10.0
var _trackers: Dictionary = {}

var _capture: bool = false
var _frames_qty: int = 0

const TRACKERS_QTY: int = 6
const TRACKER_INDS: Dictionary = {
	"0": 0, 
	"1": 1, 
	"2": 2, 
	"3": 3, 
	"4": 4, 
	"5": 5
}

# Called when the node enters the scene tree for the first time.
func _ready():
	_capture = false
	_frames_qty = 0
	
	init_visuals()
	
	var c: OpenvrCaptureNode = $Capture
	c.init( QTY )
	_trackers = enumerate_trackers()
	
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
	for ind in range(TRACKERS_QTY):
		var t: Transform = poses[index]
		t.origin *= POSITION_SCALE
		var tr = trackers.get_child( index )
		tr.transform = t
		index += 1
	
	if _capture:
		var db = $FramesDb
		db.set_frame( _frames_qty, frame )
		_frames_qty += 1
	
	#debug_print( poses )



func start_capture():
	_capture = true
	_frames_qty = 0
	var db = $FramesDb
	db.set_config( "vive_inds", _trackers )


func stop_capture():
	_capture = false


func capture_frame():
	var c: OpenvrCaptureNode = $Capture
	var frame = []
	var poses = []
	
	var qt: int = TRACKERS_QTY*7
	poses.resize(qt)
	for i in range(qt):
		frame[i] = 0.0
	
	for ind in _trackers:
		var tracker = _trackers[ind]
		var index = tracker.index
		var t: Transform = c.pose( ind )
		var r: Vector3 = t.origin
		var q: Quat    = t.basis
		var i: int = 7*index
		frame[i]   = q.w
		frame[i+1] = q.x
		frame[i+2] = q.y
		frame[i+3] = q.z
		frame[i+4] = r.x
		frame[i+5] = r.y
		frame[i+6] = r.z
		poses.push_back( t )
	
	return [poses, frame]


func init_visuals():
	var index: int = 0
	var Tr = load( "res://mm_capture/tracker.tscn" )
	var trackers: Node = $Trackers
	for ind in _trackers:
		var tr = Tr.instance()
		tr.name = "Tracker_%d" % index
		trackers.add_child( tr )
		ind += 1



func enumerate_trackers():
	var trackers: Dictionary = {}
	var qty: int = 0
	for i in range(1, QTY):
		var connected: bool = $Capture.connected( i )
		if connected:
			var serial: String = $Capture.serial( i )
			print( "index: ", i, ", serial: ", serial )
			var exists: bool = TRACKER_INDS.has( serial )
			var index: int
			if exists:
				 index = TRACKER_INDS[serial]
			else:
				index = qty
			var tracker = { index = index, serial = serial }
			trackers[i] = tracker
			qty += 1
			if qty >= 6:
				break
	
	return trackers


func debug_print( poses ):
	print( "" )
	print( "" )
	print( "" )
	var index: int = 0
	for ind in range(TRACKERS_QTY):
		var t: Transform = poses[ind]
		print( "tracker: ", ind, ", r: ", t.origin )
		index += 1



