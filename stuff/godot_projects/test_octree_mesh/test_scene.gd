extends RefFrameNode


var broad_tree: BroadTreeGd = null


func _init():
	broad_tree = BroadTreeGd.new()

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var start: Vector3 = Vector3( 0.1, 0.1, 1.0 )
	var end: Vector3   = Vector3( 0.1, 0.1, -1.0 )
	var ok: bool = broad_tree.subdivide( self )
	var ret: Array = broad_tree.intersects_segment_face( start, end, null )
