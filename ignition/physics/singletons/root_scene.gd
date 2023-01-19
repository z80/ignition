extends Node


var vp_container: Node = null

var ref_frame_root: RefFrameRoot = null


func get_visual_layer_space():
	var ret: Spatial = get_node( "VisualLayerSpace" )
	ret = ret.root
	return ret


func get_visual_layer_near():
	var root: Node = get_node( "VisualLayerNear" )
	var ret: Spatial = root.root
	return ret


func get_visual_layer_overlay():
	if vp_container == null:
		vp_container = get_node( "VisualLayerOverlay" )
	var vp: Viewport = vp_container.get_viewport()
	return vp


func get_root_for_physics_envs():
	var ret = get_node( "PhysicsEnvs" )
	return ret




func set_visual_overlay_visible( en: bool ):
	if vp_container == null:
		vp_container = get_node( "VisualLayerOverlay" )
	vp_container.visible = en


# Let's say, this one is for panels
func get_root_for_gui_panels():
	var ret: Node = get_node( "GuiPanels" )
	return ret


func get_root_for_gui_windows():
	var ret: Node = get_node( "GuiWindows" )
	return ret


func get_root_for_gui_popups():
	var ret: Node = get_node( "GuiPopups" )
	return ret




func get_unique_name_for_physics_envs( name_template: String ):
	var section: Node = get_node( "physics_envs" )
	var ret: String = _get_unique_name_for( section, name_template )
	return ret


func get_unique_name_for_visuals( name_template: String ):
	var section: Node = get_node( "visuals" )
	var ret: String = _get_unique_name_for( section, name_template )
	return ret




func _get_unique_name_for( section: Node, name_template: String ):
	var path: String = section.get_path()
	var p0: String = path + "/" + name_template
	var n0: Node = get_node_or_null( p0 )
	if n0 == null:
		return name_template
	
	var ind: int = 0
	while true:
		var name: String = name_template + "_" + str(ind)
		var p: String = path + "/" + name
		var n: Node = get_node_or_null( p )
		if n == null:
			return name
		ind += 1

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
