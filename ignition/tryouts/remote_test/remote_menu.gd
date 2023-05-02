extends Control


var HOST_IP = "127.0.0.1"
var HOST_PORT = 4321

var _player_info = {}
var _all_players = {}


# Called when the node enters the scene tree for the first time.
func _ready():
	get_tree().connect("peer_connected", Callable(self, "_player_connected"))
	get_tree().connect("peer_disconnected", Callable(self, "_player_disconnected"))
	get_tree().connect("connected_to_server", Callable(self, "_connected_ok"))
	get_tree().connect("connection_failed", Callable(self, "_connected_fail"))
	get_tree().connect("server_disconnected", Callable(self, "_server_disconnected"))


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Create_pressed():
	var peer = ENetMultiplayerPeer.new()
	peer.create_server( HOST_PORT )
	var tree: SceneTree = get_tree()
	tree.network_peer = peer
	
	_player_info = { "name": "Server player" }
	_all_players[1] = _player_info


func _on_Join_pressed():
	var peer = ENetMultiplayerPeer.new()
	peer.create_client(HOST_IP, HOST_PORT)
	var tree: SceneTree = get_tree()
	tree.network_peer = peer

	_player_info = { "name": "Client player" }


func _player_connected(id):
	# Called on both clients and server when a peer connects. Send my info to it.
	print( "Player connected # ", id )
	rpc_id( id, "register_player", _player_info )
	print( "After rpc_id() call" )

func _player_disconnected(id):
	print( "Player disconnected # ", id )
	_all_players.erase(id) # Erase player from info.

func _connected_ok():
	print( "Connected ok" )


func _server_disconnected():
	print( "Server disconnected" )


func _connected_fail():
	print( "Failed to connect" )



@rpc("any_peer") func register_player( info ):
	print( "Entered register player: ", info )
	
	var tree: SceneTree = get_tree()
	# Get the id of the RPC sender.
	var id = tree.get_remote_sender_id()
	# Store the info
	_all_players[id] = info
	
	var own_id: int = tree.get_unique_id()
	
	print( "register_player( info )" )
	print( "    own id: ", own_id )
	print( "    sent by: ", id )
	print( "    all players: " )
	print( _all_players )
	print( "\n\n\n" )







