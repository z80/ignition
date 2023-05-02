extends Node


var _players_pool: Array = []
var _sounds: Dictionary  = {}


func play(sound_path: String, music: bool = false):
	var has: bool = _sounds.has( sound_path )
	var sound: AudioStream
	if has:
		sound = _sounds[sound_path]
	else:
		sound = load( sound_path )
		_sounds[sound_path] = sound
	
	var p: AudioStreamPlayer = _get_player()
	if music:
		p.bus = "Music"
	
	else:
		p.bus = "Sounds"
	
	p.stream = sound
	p.play()
	return p


func is_playing( sound_path: String ) -> bool:
	var has: bool = _sounds.has( sound_path )
	var sound: AudioStream
	if has:
		sound = _sounds[sound_path]
	else:
		sound = load( sound_path )
		_sounds[sound_path] = sound
	
	# Get all players and see if is playing and if stream matches.
	var qty: int = get_child_count()
	for i in range(qty):
		var c: Node = get_child( i )
		var p: AudioStreamPlayer = c as AudioStreamPlayer
		if p != null:
			var playing: bool = p.playing
			if not playing:
				continue
			var stream: AudioStream = p.stream
			if stream == sound:
				return true
	
	return false


# Called when the node enters the scene tree for the first time.
func _ready():
	_find_all_players()


func _find_all_players():
	var qty: int = get_child_count()
	for i in range(qty):
		var c: Node = get_child( i )
		var p: AudioStreamPlayer = c as AudioStreamPlayer
		_players_pool.push_back( p )
		p.connect("finished", Callable(self, "_on_playback_finished").bind(p))
		


func _get_player():
	var empty: bool = _players_pool.is_empty()
	if not empty:
		var ret: AudioStreamPlayer = _players_pool.back()
		_players_pool.resize( _players_pool.size()-1 )
		return ret
	
	var p: AudioStreamPlayer = AudioStreamPlayer.new()
	p.name = _get_unique_name( "Player_" )
	add_child( p )
	p.connect("finished", Callable(self, "_on_playback_finished").bind(p))
	return p



func _get_unique_name( base: String = "Player_" ):
	var n: int = 0
	while true:
		var name: String = base + ("%02d" % n)
		var c: Node = get_node_or_null( name )
		if c == null:
			return name
		
		n += 1



func _on_playback_finished( player ):
	_players_pool.push_back( player )
	DDD.print( "released player" )


