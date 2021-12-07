
extends RefFrameNode
class_name SoundSource


# Available spare sound players.
var _sources: Array = []
# Sound resources loaded so far.
var _sound_streams: Dictionary = {}
# Sounds playing.
# Each entry has up to 3 parts.
# 1) Sound to play.
# 2) Sound to start with. It is optional.
# 3) Sound to terminate with. It is also optional.
var _sounds_played: Dictionary = {}



func play( main_sound_path: String, start_sound_path: String, stop_sound_path: String ):
	var main_sound: AudioStream  = _get_sound_resource( main_sound_path )
	var start_sound: AudioStream = _get_sound_resource( start_sound_path )
	var stop_sound: AudioStream  = _get_sound_resource( stop_sound_path )
	
	var p: AudioStreamPlayer3D = _get_stream_player()
	p.stream = main_sound
	p.play()
	
	var data: Array = [ main_sound, start_sound, stop_sound ]
	_sounds_played[p] = data





func stop( sound_path: String, immediately: bool = false ):
	var stream: AudioStream = _get_sound_resource( sound_path )
	for p in _sounds_played:
		var data: Array = _sounds_played[p]
		var has: bool = data.has( stream )
		
		if has:
			var index: int = data.find( stream )
			if index > 1:
				p.stop()
				_sources.push_back( p )
			
			else:
				p.stream = data[2]
				p.play()




func stop_all():
	for p in _sounds_played:
		p.stop()
		_sources.push_back( p )




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



func _get_stream_player():
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



func _get_sound_resource( sound_path: String ):
	var has: bool = _sound_streams.has( sound_path )
	var sound: AudioStream
	if has:
		sound = _sound_streams[sound_path]
	else:
		sound = load( sound_path )
		_sound_streams[sound_path] = sound
	return sound




func _on_playback_finished( p: AudioStreamPlayer3D ):
	var data: Array = _sounds_played[p]
	var main: AudioStream  = data[0]
	var start: AudioStream = data[1]
	var stop: AudioStream = data[2]
	var current: AudioStream = p.stream
	
	if current == main:
		if stop != null:
			p.stream = stop
			p.play()
		
		else:
			_sources.push_back( p )
			_sounds_played.erase( p )
		
	elif current == start:
		p.stream = main
		p.play()
	
	else:
		if current == stop:
			_sources.push_back( p )
			_sounds_played.erase( p )


