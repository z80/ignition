extends Node

const ENVS_QTY = 32
var envs_: Array

# Called when the node enters the scene tree for the first time.
func _ready():
	envs_.resize(ENVS_QTY)
	for i in range(ENVS_QTY):
		envs_[i] = null


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

func create_environment():
	for i in range(ENVS_QTY):
		var e = envs_[i]
		if e == null:
			e = PhysicsEnvironment.new()
			get_tree().get_root().add_child( e )
			envs_[i] = e
			return e
	return null


func release_environment( env ):
	for i in range(ENVS_QTY):
		var e = envs_[i]
		if ( e == env ):
			envs_[i] = null
			env.queue_tree()

