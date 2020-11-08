extends MeshInstance


export(bool) var enabled = false setget _set_enabled
export(float) var throttle = 0.0 setget _set_throttle


func _set_enabled( en ):
	enabled = en
	$CPUParticles.emitting = en

func _set_throttle( th ):
	throttle = th
	if throttle <= 0.0:
		_set_enabled( false )
		throttle = 0.0
	else:
		if throttle > 1.0:
			throttle = 1.0
		var velocity: float = throttle * 150.0 + 50.0
		$CPUParticles.initial_velocity = velocity

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
