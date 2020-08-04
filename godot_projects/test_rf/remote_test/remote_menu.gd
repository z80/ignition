extends Control


var HOST_IP = "127.0.0.1"
var HOST_PORT = 4321

var _player_info = {}
var _all_players = {}


# Called when the node enters the scene tree for the first time.
func _ready():
	get_tree().connect("network_peer_connected", self, "_player_connected")
	get_tree().connect("network_peer_disconnected", self, "_player_disconnected")
	get_tree().connect("connected_to_server", self, "_connected_ok")
	get_tree().connect("connection_failed", self, "_connected_fail")
	get_tree().connect("server_disconnected", self, "_server_disconnected")


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Create_pressed():
	var peer = NetworkedMultiplayerENet.new()
	peer.create_server( HOST_PORT )
	var tree: SceneTree = get_tree()
	tree.network_peer = peer
	
	_player_info = { "name": "Server player" }
	_all_players[1] = _player_info


func _on_Join_pressed():
	var peer = NetworkedMultiplayerENet.new()
	peer.create_server( HOST_PORT )
	var tree: SceneTree = get_tree()
	tree.network_peer = peer

	_player_info = { "name": "Client player" }


func _player_connected(id):
	# Called on both clients and server when a peer connects. Send my info to it.
	rpc_id( id, "register_player", _player_info )

func _player_disconnected(id):
	_all_players.erase(id) # Erase player from info.

func _connected_ok():
	pass # Only called on clients, not server. Will go unused; not useful here.

func _server_disconnected():
	pass # Server kicked us; show error and abort.

func _connected_fail():
	pass # Could not even connect to server; abort.


remote func register_player( info ):
	var tree: SceneTree = get_tree()
	# Get the id of the RPC sender.
	var id = tree.get_rpc_sender_id()
	# Store the info
	_all_players[id] = info
	
	var own_id: int = tree.get_network_unique_id()
	
	print( "register_player( info )" )
	print( "    own id: ", own_id )
	print( "    sent by: ", id )
	print( "    all players: " )
	print( _all_players )
	print( "\n\n\n" )
	
	
	
	
