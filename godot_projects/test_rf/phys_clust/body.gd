
extends Rf
class_name Body

var visual_    = null
var physical_  = null  


func create_visual():
	return null


func create_physical():
	return null


func set_origin( rf ):
	_ref_frame.set_origin( rf )


func set_root( rf ):
	_ref_frame.set_root( rf )


func set_rf_transform( t ):
	_ref_frame.set_rf_transform( t )
