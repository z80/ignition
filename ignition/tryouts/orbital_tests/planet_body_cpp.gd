extends RefFrameNode

var motion: CelestialMotionRef = null

export(NodePath) var parent
export(float) var r = 1.0
export(float) var v = 1.0
export(Vector3) var unit_r = Vector3( 1.0, 0.0, 0.0 )
export(Vector3) var unit_v = Vector3( 0.0, 0.0, -1.0 )
export(float) var ecc = 0.0
export(float) var T = 60.0

var gm: float = 1.0
var state = null

var initialized: bool = false
var orbiting: bool = false

# Called when the node enters the scene tree for the first time.
func _ready():
	init()

func init():
	if initialized:
		return
	initialized = true
	
	motion = CelestialMotionRef.new()
	init_gm( r, v )
	init_orbit()
	var mesh = get_node( "Sphere" )
	mesh.scale = Vector3( r, r ,r )
	

func init_gm( r: float, v: float ):
	r = r*0.001
	v = v*0.001
	gm = motion.init_gm( r, v )

func init_orbit():
	var p = get_node( parent )
	if p == null:
		orbiting = false
		return
	
	p.init()
	var parent_gm: float = p.gm
	T = T / 3600.0
	motion.launch_elliptic( parent_gm, unit_r, unit_v, T, ecc )
	orbiting = true

func _process(delta):
	if not orbiting:
		return
	motion.process_rf( delta, self )
	var mesh = get_node( "Sphere" )
	var t: Transform = self.t()
	mesh.transform = t
