
extends Resource
class_name CreatableBlockDesc

# A base class for all creatable objects.
# The idea is that folders are parsed automatically.
# If "desc.tres" is met, it is loaded and all descriptions 
# are obtained.

# Actual part resource name and script name are supposed to 
# match the folder name in which they reside.

@export var block_scene: PackedScene = null

@export var category: String      = "dynamic_parts"
@export var required_tech: String = "root"

@export var icon: Resource              = null
@export var brief_description: String = ""
@export var wiki_file: String         = "res://wiki/root.wiki"


func _init():
	# Probably here load/fill brief description and an icon.
	pass



# Might want to repload something asynchronously.
func preload_resources( _loader: ResourceLoader ):
	pass



# This one should call "create_internal()" with appropriate scene resource.
func create( parent: RefFrameNode, se3: Se3Ref ):
	var block: Node = create_internal( block_scene, parent, se3 )
	return block



# This one should be used unchanged by create() method.
func create_internal( scene: PackedScene, parent: RefFrameNode, se3: Se3Ref ):
	var inst: Node = scene.instantiate()
	if parent == null:
		parent = RootScene.get_root_for_bodies()
	inst.change_parent( parent, true )
	inst.init()
	inst.set_se3( se3 )
	return inst


func is_static():
	var cat: Resource = Game.find_category( category )
	if cat == null:
		return false
	
	var ret: bool = cat.is_static
	return ret







