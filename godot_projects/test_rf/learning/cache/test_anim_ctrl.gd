extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"



# Called when the node enters the scene tree for the first time.
func _ready():
	pass	


func _exit_tree():
	pass


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var ac = $AnimCtrl
	#  apply_controls( ctrl_v: Vector3, cam_t: Transform = Transform(), tps: bool = true ):
	ac.apply_controls( )


