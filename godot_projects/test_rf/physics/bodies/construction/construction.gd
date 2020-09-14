
extends Body


var t_elapsed: float = 0.0

# String describing editing mode.
var activated_mode = null

# Target being edited.
var edited_target = null
# Widget editing the target above.
var editing_widget = null

func _ready():
	#var V  = load( "res://physics/bodies/construction/visual.tscn")
	#var P  = load( "res://physics/bodies/construction/physical.tscn" )
	#var B  = load( "res://physics/bodies/construction/panel_parts/item_button.tscn" )
	#var E  = load( "res://physics/bodies/construction/gui_enter.tscn" )
	#var T  = load( "res://physics/bodies/construction/gui_transform_2.tscn" )
	#var L  = load( "res://physics/bodies/construction/gui_leave_contruction_mode.tscn" )
	#var mode = "construction"
	#var classes = .gui_classes( mode )
	#if mode == "construction":
	#	var Tr = null #load( "res://physics/bodies/construction/gui_transform.tscn" )
	#	var Lv = null #load( "res://physics/bodies/construction/gui_leave_construction_mode.tscn" )
	#	classes.push_back( Tr )
	#	classes.push_back( Lv )
	#print( "classes: ", classes )
	
	$PanelParts.construction = self
	$PanelParts.connect( "create_block", self, "create_block"  )
	
		# Also create a super body.
	super_body = ConstructionSuperBody.new()
	var p = get_parent()
	p.add_child( super_body )
	# Place own reference there.
	super_body.construction = self
	# And in the list of sub-bodies.
	super_body.sub_bodies.push_back( self )



func process_inner(delta):
	.process_inner(delta)
	
	t_elapsed += delta
	if t_elapsed >= 1.0:
		t_elapsed -= 1.0
		var player_rf = PhysicsManager.player_ref_frame
		#compute_relative_to_root( player_rf )
		#var t: Transform = self.t_root()
		#print( "Construction relative to player rf: ", t )
	
	
	# If it is active check if player is too far. If it is, deactivate.
	var do_deac: bool = check_if_deactivate()
	if do_deac:
		deactivate()



func init():
	var Visual   = load( "res://physics/bodies/construction/visual.tscn")
	var Physical = load( "res://physics/bodies/construction/physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()

	
	var t: Transform
	t.origin = Vector3( 0.0, 1.0, 0.0 )
	set_t( t )


func gui_classes( mode: String = "" ):
	var classes = []
	var common_classes = .gui_classes( mode )
	for cl in common_classes:
		classes.push_back( cl )
	
	if (mode != "construction_menu") and (mode != "construction_editing"):
		var PanelEnter = load( "res://physics/bodies/construction/gui_enter.tscn" )
		classes.push_back( PanelEnter )
	
	return classes



func process_user_input( _event: InputEvent ):
	pass




func set_collision_layer( layer ):
	if _physical:
		var rb = _physical.get_child( 0 )
		rb.collision_layer = layer
		rb.collision_mask  = layer



func activate():
	$PanelParts.visible = true
	
	activated_mode = "construction_menu"



func deactivate():
	$PanelParts.visible = true
	
	activated_mode = null


func activate_grab( body ):
	if not is_instance_valid( body ):
		return
	var Grab = load( "res://physics/bodies/construction/grab_3d.tscn" )
	var grab = Grab.instance()
	BodyCreator.root_node.add_child( grab )
	edited_target  = body
	editing_widget = grab
	grab.target = body
	activated_mode = "construction_editing"


func activate_rotate( body ):
	if not is_instance_valid( body ):
		return
	var Rotate = load( "res://physics/bodies/construction/rotate_3d.tscn" )
	var rotate = Rotate.instance()
	body.add_child( rotate )
	edited_target  = body
	editing_widget = rotate
	rotate.target = body
	activated_mode = "construction_editing"


func check_if_deactivate():
	var player = PhysicsManager.player_focus
	if player == null:
		return true
	
	# Player in this ref. fame in order to easily compoute each coordinate.
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


func create_block( block_name ):
	var block: Body = BodyCreator.create( block_name )
	if block == null:
		return
	
	var player = PhysicsManager.player_focus
	block.change_parent( player )
	var t: Transform
	t.origin = Constants.CONSTRUCTION_CREATE_AT
	block.set_t( t )
	var p = self.get_parent()
	block.change_parent( p )
	# Establish relations.
	block.super_body = self.super_body
	super_body.sub_bodies.push_back( block )
	
	# Make it selected to be able to move it.
	PhysicsManager.player_select = block


