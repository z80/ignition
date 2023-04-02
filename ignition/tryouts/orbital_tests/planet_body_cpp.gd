extends RefFrameNode

var motion: CelestialMotionRef = null

@export var parent: NodePath
@export var r: float = 1.0
@export var v: float = 1.0
@export var unit_r: Vector3 = Vector3( 1.0, 0.0, 0.0 )
@export var unit_v: Vector3 = Vector3( 0.0, 0.0, -1.0 )
@export var ecc: float = 0.0
@export var T: float = 60.0

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
	init_gm_speed( r, v )
	init_orbit()
	var mesh = get_node( "Sphere" )
	mesh.scale = Vector3( r, r ,r )
	

func init_gm_speed( r: float, v: float ):
	r = r*0.001
	v = v*0.001
	gm = motion.init_gm_speed( r, v )

func init_orbit():
	var p = get_node( parent )
	if p == null:
		orbiting = false
		return
	
	p.init()
	var parent_gm: float = p.own_gm
	T = T / 3600.0
	motion.launch_elliptic( parent_gm, unit_r, unit_v, T, ecc )
	orbiting = true

func _process(delta):
	if not orbiting:
		return
	motion.process_rf( delta, self )
	var mesh = get_node( "Sphere" )
	var t: Transform3D = self.t()
	mesh.transform = t
