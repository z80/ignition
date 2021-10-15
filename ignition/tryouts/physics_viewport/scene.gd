extends Spatial


var rf_a = null
var rf_b = null

var rb_a: RigidBody = null
var rb_b: RigidBody = null

# Called when the node enters the scene tree for the first time.
func _ready():
	rf_a = get_node( "RfA" )
	rf_b = get_node( "RfB" )
	
	var t: Transform = Transform.IDENTITY
	t.origin = Vector3( -5.0, -0.2, 0.0 )
	
	var S = load( "res://tryouts/physics_viewport/physics_obj.tscn" )
	var rb: RigidBody = S.instance()
	rb.gravity_scale = 0.0
	rb.transform = t
	rb.linear_velocity = Vector3( 1.0, 0.0, 0.0 )
	rf_a.add_physics_body( rb )
	rb_a = rb
	
	t.origin = Vector3( 5.0, 0.2, 0.0 )
	rb = S.instance()
	rb.gravity_scale = 0.0
	rb.transform = t
	rb.linear_velocity = Vector3( -1.0, 0.0, 0.0 )
	rf_b.add_physics_body( rb )
	rb_b = rb
	
	


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var boxA = get_node( "BoxA" )
	var boxB = get_node( "BoxB" )
	boxA.transform = rb_a.transform
	boxB.transform = rb_b.transform
