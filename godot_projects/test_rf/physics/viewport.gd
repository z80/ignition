extends Viewport


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	var world = self.world
	var s = world.space
	print( "space: ", s  )


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

func _physics_process(delta):
	$RigidBody.add_central_force( Vector3( 0.0, -0.5, 0.0 ) )
