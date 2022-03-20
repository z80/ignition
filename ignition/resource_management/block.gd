
extends Resource
class_name CreatableBlockDesc

# A base class for all creatable objects.
# The idea is that folders are parsed automatically.
# If "desc.tres" is met, it is loaded and all descriptions 
# are obtained.

# Actual part resource name and script name are supposed to 
# match the folder name in which they reside.

export(PackedScene) var block_scene = null

export(String) var category      = "dynamic_parts"
export(String) var required_tech = "root"

export(Resource) var icon              = null
export(String)   var brief_description = ""
export(String)   var wiki_file         = "res://wiki/root.wiki"


func _init():
	# Probably here load/fill brief description and an icon.
	pass



# Might want to repload something asynchronously.
func preload_resources( loader: ResourceInteractiveLoader ):
	pass



# This one should call "create_internal()" with appropriate scene resource.
func create( parent: RefFrameNode, se3: Se3Ref ):
	var block: Node = create_internal( block_scene, parent, se3 )
	return block



# This one should be used unchanged by create() method.
func create_internal( scene: PackedScene, parent: RefFrameNode, se3: Se3Ref ):
	var inst: Node = scene.instance()
	if parent == null:
		parent = RootScene.get_root_for_bodies()
	inst.init()
	inst.change_parent( parent )
	inst.set_se3( se3 )
	return inst










