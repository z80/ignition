
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
	
	var p: Node = get_parent()
	if p != parent:
		p.change_parent( parent )
	
	_se3.copy_from( se3 )


# Called when the node enters the scene tree for the first time.
func _ready():
	_visual = VisualScene.instance()
	var root_for_visuals: Node = RootScene.get_root_for_visuals()
	root_for_visuals.add_child( _visual )
	
	_se3 = Se3Ref.new()


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var cam: RefFrameNode = PhysicsManager.camera
	var se3: Se3Ref = self.relative_to( cam )
	
	var t: Transform = se3.transform
	_visual.transform = t
