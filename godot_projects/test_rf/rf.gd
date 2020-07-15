extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
var _ref_frame = null

# Called when the node enters the scene tree for the first time.
func _ready():
	_ref_frame = RefFrame.new()
	pass # Replace with function body.

func init( rf_tree ):
	rf_tree.push_back( _ref_frame )
	var in_tree = _ref_frame.in_tree()
	print( "in tree: ", in_tree )
	
func _process(delta):
	_ref_frame.compute_relative_to_root()
	var t: Transform = _ref_frame.t_root()
	set_transform( t )

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass



func set_origin( rf ):
	_ref_frame.set_origin( rf )


func set_root( rf ):
	_ref_frame.set_root( rf )


func set_rf_transform( t ):
	_ref_frame.set_rf_transform( t )





