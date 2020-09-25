extends ViewportContainer


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func set_camera_pose( t: Transform ):
	$Viewport/Camera.transform = t


func set_frame( f: Array ):
	var sz: int = f.size()
	var cubes = [ $Viewport/Box_00, $Viewport/Box_01, $Viewport/Box_02 ]
	var ind: int = 0
	for i in range( 0, sz, 7 ):
		var q: Quat = Quat( f[i+1], f[i+2], f[i+3], f[i] )
		var r: Vector3 = Vector3( f[i+4], f[i+5] - 1.5, f[i+6] ) * 5
		var cube: MeshInstance = cubes[ind]
		var t: Transform
		t.origin = r
		t.basis  = q
		cube.transform = t
		cube.scale = Vector3( 0.75, 0.75, 0.75 )
		ind += 1
		if ind >= 3:
			break



func _on_ViewportContainer_resized():
	$Viewport.size = self.get_rect().size






