
extends RefFrame
class_name RefFrameResourceProvider


var _visual = null
var _physicals: Dictionary

func _init():
	._init()
	_visual = create_visual()


func _process(_delta):
	._process(_delta)




func _process_physics_frames():
	pass



func _process_visual_frame():
	var rf = PhysicsManager.player_ref_frame
	update_visual( rf )


# Need to override this one and create a visual shape internally.
func create_visual():
	return null


# Need to override this one and make it draw/move visual.
func update_visual( rf ):
	pass

