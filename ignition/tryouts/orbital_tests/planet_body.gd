extends Node3D

var Motion = load( "res://physics/orbital_motion/motion.gd" )

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

# Called when the node enters the scene tree for the first time.
func _ready():
	init()

func init():
	if initialized:
		return
	initialized = true
	init_gm_speed( r, v )
	init_orbit()
	var mesh = get_node( "Sphere" )
	mesh.scale = Vector3( r, r ,r )
	

func init_gm_speed( r: float, v: float ):
	gm = Motion.init_gm_speed( r, v )

func init_orbit():
	var p = get_node( parent )
	if p == null:
		state = null
		return
	
	p.init()
	var parent_gm: float = p.own_gm
	state = Motion.init_elliptic( parent_gm, unit_r, unit_v, T, ecc )

func _process(delta):
	if state == null:
		return
	var ret: Array = Motion.process( delta, state )
	var r: Vector3 = ret[0]
	transform.origin = r
