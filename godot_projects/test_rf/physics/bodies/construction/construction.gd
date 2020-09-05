
extends Body


var t_elapsed: float = 0.0

var _activated: bool = false


func process_inner(delta):
	.process_inner(delta)
	
	t_elapsed += delta
	if t_elapsed >= 1.0:
		t_elapsed -= 1.0
		var player_rf = PhysicsManager.player_ref_frame
		#compute_relative_to_root( player_rf )
		#var t: Transform = self.t_root()
		#print( "Construction relative to player rf: ", t )
	
	
	# If it is active check if player is too far. It it is, deactivate.
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
	var PanelEnter = load( "res://physics/bodies/construction/gui_enter.tscn" )
	classes.push_back( PanelEnter )
	var common_classes = .gui_classes( mode )
	for cl in common_classes:
		classes.push_back( cl )
	
	return classes



func process_user_input( _event: InputEvent ):
	pass




func set_collision_layer( layer ):
	if _physical:
		var rb = _physical.get_child( 0 )
		rb.collision_layer = layer
		rb.collision_mask  = layer



func activate():
	# Transfer player to the center of the building zone
	var player = PhysicsManager.player_focus
	var player_rf = PhysicsManager.player_ref_frame
	self.compute_relative_to_root( player_rf )
	var t: Transform = self.t_root()
	t.origin.y += 2.0
	player.set_t( t )
	player.update_physical_state_from_rf()

	$PanelParts.visible = true
	
	_activated = true



func deactivate():
	$PanelParts.visible = true
	
	_activated = false


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

