extends RigidBody2D


export(Texture) var sprite = null setget _set_sprite, _get_sprite
var mouse_inside: bool = false

signal clicked

func start():
	self.mode = RigidBody2D.MODE_RIGID

func stop():
	self.mode = RigidBody2D.MODE_STATIC

func _set_sprite( s: Texture ):
	$Sprite.texture = s
	var u = s.get_width()
	var v = s.get_height()
	$CollisionShape.set( "shape/extents", Vector2(u,v) )
	$Area2D/CollisionShape2D.set( "shape/extents", Vector2(u,v) )

func _get_sprite():
	return $Sprite.texture

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
