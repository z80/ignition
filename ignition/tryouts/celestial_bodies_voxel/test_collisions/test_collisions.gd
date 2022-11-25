extends RefFrameNode


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	var cam: Node = get_node( "Planet/Rotation/RefFramePhysics/Character/PlayerCamera" )
	PhysicsManager.camera = cam


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
