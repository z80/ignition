
extends PhysicsBodyBase


var panel_parts: Control = null

var t_elapsed: float = 0.0
var counter: int = 0

# String describing editing mode.
var activated_mode = null

# Target being edited.
var edited_target: RefFrameNode = null
# Widget editing the target above.
var editing_widget = null

var dynamic_blocks: Array = []
var static_blocks: Array  = []

func _ready():
	init()
	




func process_inner(delta):
	.process_inner(delta)
	
	#t_elapsed += delta
	#if t_elapsed >= 1.0:
	#	t_elapsed -= 1.0
	#	print( "Construction process: ", counter )
	#	counter += 1
	
	
	# If it is active check if player is too far. If it is, deactivate.
	var do_deac: bool = check_if_deactivate()
	if do_deac:
		construction_deactivate()



func init():
	var Visual   = load( "res://physics/bodies/construction_new/visual.tscn")
	var Physical = load( "res://physics/bodies/construction_new/physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical
	
	need_super_body = true

	.init()
	
	#var t: Transform = Transform.IDENTITY
	#t.origin = Vector3( 0.0, 1.0, 0.0 )
	#set_t( t )


func gui_classes( mode: Array = [] ):
	var classes = []
	# Don't unclude standard classes as we don't want to control the contruction.
	#var common_classes = .gui_classes( mode )
	#for cl in common_classes:
	#	classes.push_back( cl )
	
	if ( not mode.has( "construction_menu" ) ) and ( not mode.has( "construction_editing" ) ):
		var PanelEnter = load( "res://physics/bodies/construction_new/gui_elements/gui_enter.tscn" )
		classes.push_back( PanelEnter )
	
	return classes



func process_user_input( _event: InputEvent ):
	pass




func set_collision_layer( layer ):
	if _physical:
		var rb = _physical.get_child( 0 )
		rb.collision_layer = layer
		rb.collision_mask  = layer



func construction_activate():
	#$PanelParts.visible = true
	var PanelParts = load( "res://physics/bodies/construction_new/panel_parts/panel_combined.tscn" )
	panel_parts = PanelParts.instance()
	#panel_parts.construction = self
	panel_parts.connect( "block_picked", self, "on_create_block" )
	panel_parts.connect( "launch",       self, "on_launch" )
	panel_parts.connect( "abort",        self, "on_abort" )
	var panel_parent: Control = RootScene.get_root_for_gui_panels()
	panel_parent.add_child( panel_parts )
	
	activated_mode = "construction_menu"
	dynamic_blocks = []



func construction_deactivate():
	if activated_mode != null:
		finish_editing()
		
		#$PanelParts.visible = false
		if (panel_parts != null) and ( is_instance_valid(panel_parts) ):
			panel_parts.queue_free()
		
		activated_mode = null
		
		_create_assembly()
		dynamic_blocks.clear()
		
		_activate_static_blocks()
		static_blocks.clear()


func activate_grab( body ):
	if not is_instance_valid( body ):
		return
	var Grab = load( "res://physics/bodies/construction_new/manip_grab/manip_grab.tscn" )
	var grab = Grab.instance()
	RootScene.get_root_for_visuals().add_child( grab )
	edited_target  = body
	editing_widget = grab
	grab.target = body
	activated_mode = "construction_editing"
	
	grab.connect( "drag_started",  self, "_on_drag_started" )
	grab.connect( "drag_finished", self, "_on_drag_finished" )


func _on_drag_started( grab: Node ):
	var target: Part = grab.target as Part
	if target != null:
		var ok: bool = target.decouple()
		if ok:
			target.play_sound( Constants.ConstructionDecoupling )
	set_show_coupling_nodes( true )


func _on_drag_finished( grab: Node ):
	var target: Part = grab.target as Part
	if target != null:
		set_show_coupling_nodes( false )
		var ok: bool = target.couple()
		if ok:
			target.play_sound( Constants.ConstructionCoupling )
		
		else:
			ok = target.couple_surface()
			if ok:
				target.play_sound( Constants.ConstructionCoupling )


func finish_editing():
	if is_instance_valid( editing_widget ):
		editing_widget.queue_free()
		edited_target  = null
	
	activated_mode = "construction_menu"






func _create_assembly():
	var qty = dynamic_blocks.size()
	if qty > 0:
		# Pick any one.
		var part: Part = dynamic_blocks[0]
		# And call Dfs to find the root one.
		var parts: Array = Part.dfs_search( part )
		part = parts[0]
		
		
		# For dubuggin check the size of the DFS result and if 
		# it is smaller than 3, repeat the process in order to see why.
		var dbg_qty: int = parts.size()
		if dbg_qty < 3:
			parts = Part.dfs_search( part )
		
		
		
		# Destroy parts which are not a part of the assembly.
		for i in range(qty):
			var p: Part = dynamic_blocks[i]
			var has: bool = parts.has( p )
			if not has:
				p.queue_free()
		
		# Assign null as a superbody.
		# Own super body will be created on the first request.
		for pt in parts:
			pt.set_super_body( null )
		part.activate()
	
	

	#if qty > 1:
	#	var sb = PartAssembly.new()
	#	for body in dynamic_blocks:
	#		sb.add_sub_body( body )
		
	#	# Need to call this one once.
	#	sb.create_edges()
	#	# Setting parent after adding all the bodies.
	#	var p = self.get_parent()
	#	sb.change_parent( p )
	#	sb.activate()
	
	#else:
	#	for body in dynamic_blocks:
	#		body.activate()


func _activate_static_blocks():
	for b in static_blocks:
		var block: StaticPhysicsBody = b
		block.activate()




func check_if_deactivate():
	var player = PhysicsManager.camera.get_parent()
	if (player == null) or ( not is_instance_valid(player) ):
		return true
	
	# Player in this ref. fame in order to easily compute each coordinate.
	var se3: Se3Ref = player.relative_to( self )
	var r: Vector3 = se3.r
	var dx: float = abs( r.x )
	var dz: float = abs( r.z )
	if dx >= Constants.CONSTRUCTION_DEACTIVATE_DIST:
		return true
	if dz >= Constants.CONSTRUCTION_DEACTIVATE_DIST:
		return true
	
	return false


func create_block( block_desc: Resource ):
	var player = PhysicsManager.camera.get_parent()
	
	var t: Transform = Transform.IDENTITY
	t.origin = Constants.CONSTRUCTION_CREATE_AT
	var se3: Se3Ref = Se3Ref.new()
	se3.transform = t
	se3.v = Vector3.ZERO
	se3.w = Vector3.ZERO
	
	var p: Node = self.get_parent()

	var block: PhysicsBodyBase = block_desc.create( p, se3 )
	if block == null:
		return
	
	# This one makes it not delete superbody on activation.
	block.construction_state = PhysicsBodyBase.ConstructionState.CONSTRUCTION
	block.body_state         = PhysicsBodyBase.BodyState.KINEMATIC
	
	block.set_se3( se3 )
	
	# Make it selected to be able to move it.
	PhysicsManager.player_select = block
	
	if true: #dynamic:
		dynamic_blocks.push_back( block )
	
	else:
		static_blocks.push_back( block )
	
	
	# Disable physics to prevent blocks from flying around.
	block.deactivate()
	
	# Establish relations.
	# This one is needed in ordr to 
	# have the right gui elements in the context menu.
	var sb: Node = get_super_body()
	sb.add_sub_body( block )


func delete_block( block: PhysicsBodyBase ):
	dynamic_blocks.erase( block )
	static_blocks.erase( block )
	var sb: Node = get_super_body()
	sb.remove_sub_body( block )
	block.queue_free()



func set_show_coupling_nodes( en: bool ):
	var qty: int = dynamic_blocks.size()
	for i in range(qty):
		var p: RefFrameNode = dynamic_blocks[i]
		p.set_show_node_visuals( en )



func create_super_body():
	var sb = ConstructionSuperBodyNew.new()
	var p = get_parent()
	sb.change_parent( p )
	# Place own reference there.
	sb.construction = self
	# And in the list of sub-bodies.
	sb.add_sub_body( self )
	
	return sb





func on_create_block( block_desc: Resource ):
	create_block( block_desc )



func on_launch():
	construction_deactivate()




func on_abort():
	# Need to delete all blocks.
	construction_deactivate()






