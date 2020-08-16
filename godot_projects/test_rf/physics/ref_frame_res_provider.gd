
extends RefFrame
class_name RefFrameResourceProvider


var _visual = null
var _physicals: Dictionary

func _init():
	._init()
	_visual = create_visual()


func _process(_delta):
	._process(_delta)
	_process_physics_frames()
	_process_visual_frame()



func _process_physics_frames():
	# There might be 3 cases.
	# 1) New physics frame showed up and it wasn't provided with physics 
	#    object yes.
	# 2) It already has a physics object. Just need to update it.
	# 3) There is a physics object but no ref. frame.
	
	var bit_rfs = PhysicsManager.physics_ref_frames()
	
	# 1) New physics frames processing. I.e. the ones which don't 
	#    have physics object yet.
	for bit in bit_rfs:
		var rf = bit_rfs[bit]
		var has_physical = _physicals.has( bit )
		if has_physical:
			continue
		
		var ph = create_physical( bit, rf )
		if not ph:
			continue
		_physicals[bit] = ph
	
	
	# 2) physical object existsfor this physical layer.
	for bit in bit_rfs:
		var rf = bit_rfs[bit]
		var has_physical = _physicals.has( bit )
		if not has_physical:
			continue
		
		var ph = _physicals[bit]
		update_physical( ph, rf )
	
	# 3) Physical object exists but no appropriate ref. frame anymore.
	var bits_to_remove = []
	for bit in _physicals:
		var rf_exists = bit_rfs.has( bit )
		if rf_exists:
			continue
		
		bits_to_remove.push_back( bit )
	
	for bit in bits_to_remove:
		var ph = _physicals[bit]
		_physicals.erase( bit )
		destroy_physical( ph )





# Here just get current player ref. frame and move or 
# re-generate visual object depending on what happened with 
func _process_visual_frame():
	var rf = PhysicsManager.player_ref_frame
	update_visual( rf )








# Below are methods needed to be implemented in the derived class 
# in order to make use of object of this class.



# Need to override this one and create a visual shape internally.
func create_visual():
	return null


# Need to override this one and make it draw/move visual.
func update_visual( _rf ):
	pass

# Creating physical object(s) and assign collision bit.
# Make it relative to provided ref. frame.
func create_physical( bit, rf ):
	return null

# Update physical ref frame. Need to check if ref. frame moved
# And if it did beyond of collision object boundaries might need to 
# re-generate physics object appropriately.
func update_physical( physical, rf ):
	pass

# Ref. frame is no longer physical or completely removed.
# So this physical is no longer needed.
func destroy_physical( physical ):
	pass

# Might want to reuse physical. It requires to change its collision layer.
func change_layer_physical( physical, new_layer ):
	pass
