extends Control

var _target_obj = null
var _parent_gui = null

var _buttons: Array = []


func init( target_obj, parent_gui ):
	_target_obj = target_obj
	_parent_gui = parent_gui
	_update_from_object()


# Called when the node enters the scene tree for the first time.
func _ready():
	pass

func _process( _delta: float ):
	pass





func _update_from_object():
	var real_obj: bool = is_instance_valid( _target_obj )
	if not real_obj:
		return
	
	# Remove old buttons
	_remove_buttons()
	
	var container: VBoxContainer = get_node( "Container" )
	
		# Check if there are characters outside.
	# If there are characters close, make a button which lets them inside.
	var character: Part = _characters_for_boarding()
	if character != null:
		var b: Button = Button.new()
		b.text = "Let in"
		b.rect_min_size = Vector2( 70, 20 )
		b.connect( "pressed", self, "_get_inside_pressed", [character] )
		container.add_child( b )
	
	
	# Get list of characters inside.
	# And create appropriate number of buttons to let them out.
	var characters: Array = _target_obj.characters_inside
	var ind: int = 1
	_buttons = []
	for c in characters:
		var stri = "character #" + str(ind)
		ind += 1
		var b: Button = Button.new()
		b.text = stri
		b.connect( "pressed", self, "_get_outside_pressed", [b] )
		container.add_child( b )
		_buttons.push_back( b )



func _remove_buttons():
	for b in _buttons:
		var btn: Button = b
		if btn != null:
			btn.queue_free()
	
	_buttons.clear()



func _get_outside_pressed( button ):
	var ind: int = _buttons.find( button )
	if ind < 0:
		print( "EVA failed. Button is not in the list." )
		return
	
	var real_obj: bool = is_instance_valid( _target_obj )
	if not real_obj:
		print( "Target habitat does not exist." )
		return
	
	_target_obj.let_character_out( ind )
	_parent_gui.queue_free()



func _get_inside_pressed( character ):
	_target_obj.let_character_in( character )
	_parent_gui.queue_free()




func _characters_for_boarding():
	var real_obj: bool = is_instance_valid( _target_obj )
	if not real_obj:
		print( "Target habitat does not exist." )
		return
	
	var characters = _target_obj.characters_for_boarding()
	if characters == null:
		return null
	if characters.empty():
		return null
	
	return characters[0]







