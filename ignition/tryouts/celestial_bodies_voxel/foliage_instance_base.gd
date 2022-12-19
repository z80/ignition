
extends RefFrameNode
class_name FoliageInstanceBase

# It currently has no collisions and only 
# has visual.

export(PackedScene) var VisualScene = null

var _visual: Spatial = null

# Pose with respect to specified parent.
var _se3: Se3Ref          = null
var _parent: RefFrameNode = null


func place( parent: RefFrameNode, se3: Se3Ref ):
	_parent = parent
	if _se3 == null:
		_se3 = Se3Ref.new()
	_se3.copy_from( se3 )


# Called when the node enters the scene tree for the first time.
func _ready():
	_create_objects()

func _enter_tree():
	_create_objects()
	set_se3( _se3 )


func _create_objects():
	if _visual == null:
		_visual = VisualScene.instance()
		var root_for_visuals: Node = RootScene.get_root_for_visuals()
		root_for_visuals.add_child( _visual )
	
	if _se3 == null:
		_se3 = Se3Ref.new()


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var cam: RefFrameNode = RootScene.ref_frame_root.player_camera
	var se3: Se3Ref = self.relative_to( cam )
	
	var t: Transform = se3.transform
	_visual.transform = t
