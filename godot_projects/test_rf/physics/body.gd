
extends RefFrameNode
class_name Body

const GROUP_NAME: String = "bodies"

var _visual    = null
var _physical  = null  

func _ready():
	add_to_group( GROUP_NAME )

func _process( _delta ):
	if _visual:
		var t: Transform = self.t()
		_visual.transform = t


func _physics_process( delta ):
	if _physical:
		var t: Transform = _physical.transform
		var v: Vector3   = _physical.linear_velocity
		var w: Vector3   = _physical.angular_velocity
		self.set_t( t )
		self.set_v( v )
		self.set_w( w )


func _create_visual( Visual ):
	if _visual:
		return _visual
		
	var v = Visual.instance()
	
	var t: Transform = self.t()
	v.transform = t
	
	get_tree().get_root().add_child( v )
	_visual = v



func _create_physical( Physical ):
	if _physical:
		return _physical
	
	var p = Physical.instance()
	
	var t: Transform = self.t()
	p.transform = t
	
	get_tree().get_root().add_child( p )
	_physical = p
	
	return _physical



func set_contact_layer( layer ):
	if _physical:
		_physical.set_contact_layer( layer )


func remove_physical():
	if _physical != null:
		_physical.queue_free()
		_physical = null

