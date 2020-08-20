tool

extends RefFrameResourceProvider

var LandscapeVisual   = preload( "res://physics/landscape/landscape_visual.tscn")
var LandscapePhysical = preload( "res://physics/landscape/landscape_physical.tscn")


# Need to override this one and create a visual shape internally.
func create_visual():
	var v = LandscapeVisual.instance()
	return v


# Need to override this one and make it draw/move visual.
func update_visual( _rf ):
	_visual.update()

# Creating physical object(s) and assign collision bit.
# Make it relative to provided ref. frame.
func create_physical( bit, rf ):
	var ph = LandscapePhysical.instance()
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


