extends RefFrameNode


var _sources: Array = []








# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	_position_sources()




func _get_listener_node():
	var v: Viewport = get_viewport()
	if v == null:
		return null
	var c: Camera = v.get_camera()
	if c == null:
		return null
	var p: RefFrameNode = c.owner
	return p


func _position_sources():
	var l: RefFrameNode = _get_listener_node()
	var se3: Se3Ref = relative_to( l )
	var t: Transform = se3.transform
	
	var qty: int = get_child_count()
	for i in range(qty):
		var c: Node = get_child( i )
		var p: AudioStreamPlayer3D = c as AudioStreamPlayer3D
		if p == null:
			continue
		
		p.transform = t



func _new_source():
	var available: bool = not _sources.empty()
	if available:
		var p: AudioStreamPlayer3D = _sources.back()
		_sources.resize( _sources.size() - 1 )
		return p
	
	else:
		var p: AudioStreamPlayer3D = AudioStreamPlayer3D.new()
		var name: String = _get_unique_name()
		p.name = name
		add_child( p )
		p.connect( "finished", self, "_on_playback_finished", [p] )
		return p




func _get_unique_name( base: String = "Player_" ):
	var n: int = 0
	while true:
		var name: String = base + ("%02d" % n)
		var c: Node = get_node_or_null( name )
		if c == null:
			return name
		
		n += 1






