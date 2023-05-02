
extends RefFrameResourceProvider

var LandscapeVisual
var LandscapePhysical

func initialize():
	super.initialize()
	LandscapeVisual   = preload( "res://physics/landscape/landscape_visual.tscn")
	LandscapePhysical = preload( "res://physics/landscape/landscape_physical.tscn")


func init_force_source():
	super.init_force_source()
	force_source = ForceSourceGravityDown.new()


# Need to override this one and create a visual shape internally.
func create_visual():
	var v = LandscapeVisual.instantiate()
	if v != null:
		self.add_child( v )
	return v


# Need to override this one and make it draw/move visual.
func update_visual( rf ):
	var t: Transform3D = rf.t_root()
	_visual.update( t )

# Creating physical object(s) and assign collision bit.
# Make it relative to provided ref. frame.
func create_physical( bit, rf ):
	var ph = LandscapePhysical.instantiate()
	if ph != null:
		ph.set_collision_bit( bit )
		add_child( ph )
	return ph

# Update physical ref frame. Need to check if ref. frame moved
# And if it did beyond of collision object boundaries might need to 
# re-generate physics object appropriately.
func update_physical( physical, rf ):
	var t: Transform3D = rf.t_root()
	physical.update( t )

# Ref. frame is no longer physical or completely removed.
# So this physical is no longer needed.
func destroy_physical( physical ):
	physical.queue_free()

# Might want to reuse physical. It requires to change its collision layer.
func change_layer_physical( physical, new_layer ):
	physical.set_collision_bit( new_layer )


