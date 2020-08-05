extends Spatial


var objs = []
var enabled = false

var _t = 0.0

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	_t += delta
	var t: SceneTree = get_tree()
	var id: int = t.get_network_unique_id()
	for obj in objs:
		var is_master = obj.is_network_master()
		if is_master:
			var x = sin( _t/6.28 ) * 10.0
			var r: Vector3 = Vector3( x, 0.0, 0.0 )
			obj.rpc( "move_to", r )
	

remotesync func create_object( id ):
	var t: SceneTree = get_tree()
	var S = preload( "res://remote_test/test_scene.tscn" )
	var inst = S.instance()
	inst.set_name( "node_" + str(id) )
	inst.set_network_master( id )
	
	$Spatial.add_child( inst )

