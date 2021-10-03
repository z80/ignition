
extends Body


var t_elapsed: float = 0.0
var counter: int = 0

# String describing editing mode.
var activated_mode = null

# Target being edited.
var edited_target: RefFrameNode = null
# Widget editing the target above.
var editing_widget = null

var dynamic_blocks: Array = []

func _ready():
	init()
	
	$PanelParts.construction = self
	$PanelParts.connect( "create_block", self, "create_block" )




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
	hint_text = "Vehicle construction area. Activate \n" + "it from the menu in order to be able \n" + "to build crafts."
	var Visual   = load( "res://physics/bodies/construction/visual.tscn")
	var Physical = load( "res://physics/bodies/construction/physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

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
		var PanelEnter = load( "res://physics/bodies/construction/gui_elements/gui_enter.tscn" )
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
	$PanelParts.visible = true
	
	activated_mode = "construction_menu"
	
	dynamic_blocks = []



func construction_deactivate():
	if activated_mode != null:
		finish_editing()
		$PanelParts.visible = false
		activated_mode = null
		
		_cleanup_arguments()
		_create_assembly()
		dynamic_blocks.clear()


func activate_grab( body ):
	if not is_instance_valid( body ):
		return
	var Grab = load( "res://physics/bodies/construction/manip_grab.tscn" )
	var grab = Grab.instance()
	RootScene.get_root_for_visuals().add_child( grab )
	edited_target  = body
	editing_widget = grab
	grab.target = body
	edited_target.decouple()
	set_show_coupling_nodes( true )
	activated_mode = "construction_editing"





func finish_editing():
	if is_instance_valid( editing_widget ):
		editing_widget.queue_free()
		
		edited_target.couple()
		set_show_coupling_nodes( false )
		edited_target  = null
	
	activated_mode = "construction_menu"



func _cleanup_arguments():
	for b in dynamic_blocks:
		var body: Body = b



func _create_assembly():
	var qty = dynamic_blocks.size()
	if qty > 0:
		# Pick any one.
		var part: Part = dynamic_blocks[0]
		# And call Dfs to find the root one.
		var ret: Array = Part.dfs_search( part )
		part = ret[0]
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



func check_if_deactivate():
	var player = PhysicsManager.player_control
	if player == null:
		return true
	
	# Player in this ref. fame in order to easily compute each coordinate.
	player.compute_relative_to_root( self )
	var t: Transform = player.t_root()
	var r: Vector3 = t.origin
	var dx: float = abs( r.x )
	var dz: float = abs( r.z )
	if dx >= Constants.CONSTRUCTION_DEACTIVATE_DIST:
		return true
	if dz >= Constants.CONSTRUCTION_DEACTIVATE_DIST:
		return true
	
	return false


func create_block( block_name, dynamic: bool = false ):
	var block: Body = BodyCreator.create( block_name )
	if block == null:
		return
	
	# This one makes it not delete superbosy on activation.
	block.mode = Part.PartMode.CONSTRUCTION
	block.body_state = Body.BodyState.KINEMATIC
	
	var player = PhysicsManager.player_control
	block.change_parent( player )
	var t: Transform = Transform.IDENTITY
	t.origin = Constants.CONSTRUCTION_CREATE_AT
	block.set_t( t )
	block.set_v( Vector3.ZERO )
	block.set_w( Vector3.ZERO )
	var p = self.get_parent()
	block.change_parent( p )
	
	# Make it selected to be able to move it.
	PhysicsManager.player_select = block
	
	if dynamic:
		dynamic_blocks.push_back( block )
	
	
	# Disable physics to prevent blocks from flying around.
	block.deactivate()
	
	# Establish relations.
	# This one is needed in ordr to 
	# have the right gui elements in the context menu.
	super_body.add_sub_body( block )


func delete_block( block: Body ):
	dynamic_blocks.erase( block )
	super_body.remove_sub_body( block )
	block.queue_free()



func set_show_coupling_nodes( en: bool ):
	var qty: int = dynamic_blocks.size()
	for i in range(qty):
		var p: RefFrameNode = dynamic_blocks[i]
		p.set_show_node_visuals( en )



func create_super_body():
	var sb = ConstructionSuperBody.new()
	var p = get_parent()
	sb.change_parent( p )
	# Place own reference there.
	sb.construction = self
	# And in the list of sub-bodies.
	sb.add_sub_body( self )
	
	return sb




