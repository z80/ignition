extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var frame = $MmCaptureThree.frame()
	if frame.size() > 0:
		var mmv = $MotionMatchingVive
		mmv.generate_controls( frame )
		
		var f = mmv._f
		$Dn.set_frame( f )


