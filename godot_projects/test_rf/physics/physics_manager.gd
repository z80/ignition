extends Node

const ENVS_QTY = 32
var envs_: Array

# Called when the node enters the scene tree for the first time.
func _ready():
	envs_.resize(ENVS_QTY)
	for i in range(ENVS_QTY):
		envs_[i] = null


func acquire_environment( ref_frame ):
	for i in range(ENVS_QTY):
		var e = envs_[i]
		if e == null:
			envs_[i] = ref_frame
			return i
	return -1


func release_environment( ref_frame ):
	for i in range(ENVS_QTY):
		var e = envs_[i]
		if ( e == ref_frame ):
			envs_[i] = null


func available_qty():
	var qty: int = 0
	for i in range(ENVS_QTY):
		var e = envs_[i]
		if e == null:
			qty += 1
			
	return qty

