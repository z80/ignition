
extends PhysicsBodyBase
class_name Construction

@export var DragScene: PackedScene = null

var panel_parts: Control = null

var t_elapsed: float = 0.0
var counter: int = 0

# String describing editing mode.
var activated_mode: String = ""

# Target being edited.
var edited_target: RefFrameNode = null
# Widget editing the target above.
var editing_widget = null

var dynamic_blocks: Array = []
var static_blocks: Array  = []

func _ready():
	super()



func process_inner(delta):
	super.process_inner(delta)
	
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
	super.init()
	call_deferred( "_apply_default_orientation" )
	#var t: Transform = Transform.IDENTITY
	#t.origin = Vector3( 0.0, 1.0, 0.0 )
	#set_t( t )



func _apply_default_orientation():
	var ClosestCelestialBody = preload( 'res://physics/utils/closest_celestial_body.gd' )
	var celestial_body: RefFrameNode = ClosestCelestialBody.closest_celestial_body( self )
	if not is_instance_valid( celestial_body ):
		return
	
	var se3_rel: Se3Ref = self.relative_to( celestial_body )
	var wanted_up: Vector3 = se3_rel.r.normalized()
	
	var co: float = wanted_up.y
	var si: float = Vector2( wanted_up.x, wanted_up.z ).length()
	var elevation: float = atan2( si, co )
	var q_el: Quaternion = Quaternion( Vector3.RIGHT, elevation )
	
	co = wanted_up.z
	si = wanted_up.x
	var azimuth: float = atan2( si, co )
	var q_az: Quaternion = Quaternion( Vector3.UP, azimuth )
	
	var q_total: Quaternion = q_az * q_el
	
	var se3: Se3Ref = get_se3()
	se3.q = q_total
	set_se3( se3 )


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
	panel_parts = PanelParts.instantiate()
	#panel_parts.construction = self
	var _ok: int = panel_parts.connect("block_picked", Callable(self, "on_create_block"))
	_ok = panel_parts.connect("launch", Callable(self, "on_launch"))
	_ok = panel_parts.connect("abort", Callable(self, "on_abort"))
	var panel_parent: Control = RootScene.get_root_for_gui_panels()
	panel_parent.add_child( panel_parts )
	
	activated_mode = "construction_menu"
	dynamic_blocks = []



func construction_deactivate():
	if activated_mode.length() > 1:
		finish_editing()
		
		#$PanelParts.visible = false
		if (panel_parts != null) and ( is_instance_valid(panel_parts) ):
			panel_parts.queue_free()
		
		activated_mode = ""
		
		_activate_parts()
		dynamic_blocks.clear()
		
		_activate_static_blocks()
		static_blocks.clear()


func activate_grab( body, grab ):
	edited_target  = body
	editing_widget = grab
	grab.target = body
	activated_mode = "construction_editing"
	
	grab.connect("drag_started", Callable(self, "_on_drag_started"))
	grab.connect("drag_finished", Callable(self, "_on_drag_finished"))


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






func _activate_parts():
		# Assign null as a superbody.
		# Own super body will be created on the first request.
		#for pt in parts:
		#	pt.set_assembly( null )
		var qty: int = get_child_count()
		var parts: Array = []
		for i in range(qty):
			var part: RefFrameNode = get_child(0) as RefFrameNode
			if part != null:
				parts.push_back( part )
		
		for part in parts:
			var rf: RefFrameNode = self.get_ref_frame_physics()
			part.change_parent( rf, true )
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
	var player = RootScene.ref_frame_root.player_camera.get_parent()
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
	var player: RefFrameNode = get_ref_frame_root().player_camera.get_parent()
	
	var t: Transform3D = Transform3D.IDENTITY
	t.origin = Constants.CONSTRUCTION_CREATE_AT
	var se3: Se3Ref = Se3Ref.new()
	se3.transform = t
	se3.v = Vector3.ZERO
	se3.w = Vector3.ZERO
	
	se3 = self.relative_to_se3( player, se3 )
	se3 = se3.inverse()
	
	var block: PhysicsBodyBase = block_desc.create( self, se3 )
	if block == null:
		return
	
	#block.debug = true
	
	var is_static_block: bool = block_desc.is_static()
	
	# This one makes it not delete superbody on activation.
	block.body_state  = PhysicsBodyBase.BodyState.CONSTRUCTION
	
	# Make it selected to be able to move it.
	RootScene.ref_frame_root.player_select = block
	
	if is_static_block:
		static_blocks.push_back( block )
	
	else:
		dynamic_blocks.push_back( block )
	
	# Disable physics to prevent blocks from flying around.
	block.deactivate()
	se3.transform.origin = Vector3( 0.0, 10.0, 0.0 )
	block.set_se3( se3 )


func delete_block( block: PhysicsBodyBase ):
	dynamic_blocks.erase( block )
	static_blocks.erase( block )
	block.queue_free()



func set_show_coupling_nodes( en: bool ):
	var qty: int = dynamic_blocks.size()
	for i in range(qty):
		var p: RefFrameNode = dynamic_blocks[i]
		if p.has_method( "set_show_node_visuals" ):
			p.set_show_node_visuals( en )



func create_assembly():
	var sb = ConstructionSuperBodyNew.new()
	var p = get_parent()
	sb.change_parent( p, false )
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
	for b in static_blocks:
		b.queue_free()
	
	static_blocks.clear()
	
	for b in dynamic_blocks:
		b.queue_free()
	
	dynamic_blocks.clear()

	construction_deactivate()






