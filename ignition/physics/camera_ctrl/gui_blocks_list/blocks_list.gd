
extends Control

@export var ButtonScene: PackedScene = null
@export var Grab: PackedScene        = null

var _container: FlowContainer        = null

# Target being edited.
var _edited_target: RefFrameNode     = null
# Widget editing the target above.
var _editing_widget: Node            = null

var _activated_mode = null


# Called when the node enters the scene tree for the first time.
func _ready():
	_activated_mode = "construction_menu"
	_fill_blocks()



func _get_container():
	if _container == null:
		_container = get_node( "Container" )
	return _container


func _clear():
	var vb: FlowContainer = _get_container()
	var qty: int = vb.get_child_count()
	for i in range(qty):
		var n: Node = vb.get_child( i )
		n.queue_free()



func _fill_blocks():
	var cats: Array = Game.get_categories()
	for cat in cats:
		var blocks: Array = Game.get_blocks( cat )
		for p in blocks:
			_create_button(p)

	


func _create_button( block_desc: Resource ):
	var vb: FlowContainer = _get_container()
	var btn: Control      = ButtonScene.instantiate()
	btn.block_desc = block_desc
	vb.add_child( btn )
	btn.connect("block_picked", Callable(self, "on_block_picked"))
	
	




func on_block_picked( block_desc: Resource ):
	var player: RefFrameNode = RootScene.ref_frame_root.player_camera.get_parent()
	
	var t: Transform3D = Transform3D.IDENTITY
	t.origin = Constants.CONSTRUCTION_CREATE_AT
	var se3: Se3Ref = Se3Ref.new()
	se3.transform = t
	se3.v = Vector3.ZERO
	se3.w = Vector3.ZERO
	
	var ref_frame: RefFrameNonInertialNode = player.get_parent()
	
	se3 = ref_frame.relative_to_se3( player, se3 )
	se3 = se3.inverse()
	
	
	var block: PhysicsBodyBase = block_desc.create( ref_frame, se3 )
	if block == null:
		return
	
	queue_free()
	#return
	
#	var is_static_block: bool = block_desc.is_static()
	
	# This one makes it not delete superbody on activation.
#	block.body_state         = PhysicsBodyBase.BodyState.CONSTRUCTION
	
#	block.set_se3( se3 )
	
	#_activate_grab( block )
	
#	self.visible = false


func _activate_grab( body ):
	if not is_instance_valid( body ):
		return
	var grab = Grab.instantiate()
	RootScene.get_root_for_gui_popups().add_child( grab )
	_edited_target  = body
	_editing_widget = grab
	grab.target = body
	_activated_mode = "construction_editing"
	
	body.set_meta( "grab", grab )




func finish_editing():
	if is_instance_valid( _editing_widget ):
		_editing_widget.queue_free()
		_edited_target  = null
	
	_activated_mode = "construction_menu"
	self.queue_free()


func _on_drag_started( grab: Node ):
	var target: Part = grab.target as Part
	if target != null:
		var ok: bool = target.decouple()
		if ok:
			target.play_sound( Constants.ConstructionDecoupling )
	#set_show_coupling_nodes( true )


func _on_drag_finished( grab: Node ):
	pass





