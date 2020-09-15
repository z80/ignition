extends Spatial

export(bool) var enabled = false

var capture = null

const QTY: int = 64
const POSITION_SCALE: float = 1.0
var _trackers: Dictionary = {}

var _capture: bool = false
var _frames_qty: int = 0

var _latest_frame: Array = []

const TRACKERS_QTY: int = 3
const TRACKER_INDS: Dictionary = {
	#"LHB-4346B6EA": 0, # This is probably base station
	"LHR-D2186009": 0, 
	"LHR-DA64B439": 1, 
	"LHR-726ECDB3": 2, 
}


func frame():
	return _latest_frame


# Called when the node enters the scene tree for the first time.
func _ready():
	if not enabled:
		return
	
	var c: OpenvrCaptureNode = $Capture
	c.init( QTY )
	_trackers = enumerate_trackers()
	init_visuals()
	


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _physics_process(delta):
	if not enabled:
		return

	var c: OpenvrCaptureNode = $Capture
	var res: bool = c.process()
	if not res:
		print( "fail" )
	
	
	var result = capture_frame()
	var poses = result[0]
	var frame = result[1]
	
	_latest_frame = frame
	
	for ind in _trackers:
		var tracker = _trackers[ind]
		var index = tracker.index
		var t: Transform = poses[index]
		t.origin *= POSITION_SCALE
		var tr = tracker.tracker
		if tr == null:
			continue
		tr.transform = t

	
	#debug_print( poses )



func start_capture():
	_capture = true
	_frames_qty = 0
	var db = $FramesDb
	db.cleanup()
	db.set_config( "vive_inds", _trackers )


func stop_capture():
	_capture = false


func capture_frame():
	var c: OpenvrCaptureNode = $Capture
	var frame = []
	var poses = []
	
	var sz: int = 0
	for ind in _trackers:
		var tr = _trackers[ind]
		var index = tr.index
		if sz < index:
			sz = index
	sz += 1
	
	var qt: int = sz*7
	poses.resize(sz)
	frame.resize(qt)
	
	for ind in _trackers:
		var tracker = _trackers[ind]
		var index = tracker.index
		var t: Transform = c.pose( ind )
		
		# Validate serial number.
		var serial: String = tracker.serial
		
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
		poses[index] = t
	
	return [poses, frame]


func init_visuals():
	var Tr = load( "res://mm_capture/one_tracker_small.tscn" )
	var trackers: Node = $Trackers
	for ind in _trackers:
		var tracker = _trackers[ind]
		var index  = tracker.index
		var serial = tracker.serial
		var tr = Tr.instance()
		tr.name = "Tracker_%d" % index
		tr.text = String( index ) + " (" + serial + "), loc: " + String( ind )
		tracker.tracker = tr
		trackers.add_child( tr )



func enumerate_trackers():
	var trackers: Dictionary = {}
	var qty: int = 0
	for i in range(1, QTY):
		var connected: bool = $Capture.connected( i )
		if connected:
			var serial: String = $Capture.serial( i )
			print( "index: ", i, ", serial: ", serial )
			var exists: bool = TRACKER_INDS.has( serial )
			if not exists:
				continue
			var index: int = TRACKER_INDS[serial]
			print( "                    assigned index: ", index )
			var tracker = { index = index, serial = serial, tracker = null }
			trackers[i] = tracker
			qty += 1
			if qty >= TRACKERS_QTY:
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



func debug_print_positions():
	var c: OpenvrCaptureNode = $Capture
	
	for ind in _trackers:
		var tracker = _trackers[ind]
		var index = tracker.index
		var t: Transform = c.pose( ind )
		
		# Validate serial number.
		var serial: String = tracker.serial
		
		var r: Vector3 = t.origin
		var q: Quat    = t.basis
		
		print( "global index: ", ind, ", serial: ", serial, ", index: ", index, ", position: ", r )
	
	print( "" )
