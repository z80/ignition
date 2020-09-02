
extends Body


var t_elapsed: float = 0.0


func process_inner(delta):
	.process_inner(delta)
	
	t_elapsed += delta
	if t_elapsed >= 1.0:
		t_elapsed -= 1.0
		var player_rf = PhysicsManager.player_ref_frame
		compute_relative_to_root( player_rf )
		var t: Transform = self.t_root()
		print( "Construction relative to player rf: ", t )




func init():
	var Visual   = load( "res://physics/bodies/construction/visual.tscn")
	var Physical = load( "res://physics/bodies/construction/physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()

	
	var t: Transform
	t.origin = Vector3( 0.0, 0.0, 0.0 )
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
