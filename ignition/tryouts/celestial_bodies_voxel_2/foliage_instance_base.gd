
extends RefFrameNode
class_name FoliageInstanceBase

# It currently has no collisions and only 
# has visual.

@export var VisualScene: PackedScene = null
@export var scale: float = 1.0

var _visual: Node3D = null

# Pose with respect to specified parent.
var _se3: Se3Ref          = null
var _parent: RefFrameNode = null


func place( parent: RefFrameNode, se3: Se3Ref ):
	_parent = parent
	if _se3 == null:
		_se3 = Se3Ref.new()
	_se3.copy_from( se3 )
	
	change_parent( parent, false )
	set_se3( _se3 )



# Called when the node enters the scene tree for the first time.
func _ready():
	_create_objects()


func _enter_tree():
	_create_objects()
	set_se3( _se3 )


func _exit_tree():
	if (_visual != null) and is_instance_valid(_visual):
		_visual.queue_free()


func _create_objects():
	if _visual == null:
		_visual = VisualScene.instantiate()
		var root_for_visuals: Node = RootScene.get_visual_layer_near()
		root_for_visuals.add_child( _visual )
	
	if _se3 == null:
		_se3 = Se3Ref.new()


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _ign_pre_process(delta):
	var cam: RefFrameNode = RootScene.ref_frame_root.player_camera
	var se3: Se3Ref = self.relative_to( cam )
	
	var t: Transform3D = se3.transform
	#t = t.scaled( Vector3( scale, scale, scale ) )
	_visual.transform = t
