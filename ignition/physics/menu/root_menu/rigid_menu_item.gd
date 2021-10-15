extends RigidBody2D


export(Vector2) var size setget _set_size, _get_size
var mouse_inside: bool = false

signal clicked

func start():
	self.mode = RigidBody2D.MODE_RIGID

func stop():
	self.mode = RigidBody2D.MODE_STATIC

func _set_size( s: Vector2 ):
	$CollisionShape2D.set( "shape/extents", s )
	$Button.rect_size = s
	$Button.rect_position = s * 0.5

func _get_size():
	var s = $Button.rect_size
	return s

# Called when the node enters the scene tree for the first time.
func _ready():
	stop()


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_mouse_entered():
	mouse_inside = true


func _on_mouse_exited():
	mouse_inside = false

func _input_event(viewport, event, shape_idx):
	pass

func _on_input_event(viewport, event, shape_idx):
	if event is InputEventMouseButton:
		if event.button_index == BUTTON_LEFT and (not event.pressed):
			if mouse_inside:
				emit_signal( "clicked" )
				print( "clicked" )


func _on_TextureButton_pressed():
	emit_signal( "clicked" )
	print( "clicked (pressed button)" )
