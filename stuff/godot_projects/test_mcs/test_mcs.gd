extends Spatial


var source: VolumeSourceScriptGd = null
var cubes: MarchingCubesGd = null


# Called when the node enters the scene tree for the first time.
func _ready():
	source = VolumeSourceScriptGd.new()
	var script: Resource = preload( "res://volume_source_sphere.gd" )
	source.set_script( script )
	
	cubes = MarchingCubesGd.new()
	cubes.max_nodes_qty = 20000
	cubes.subdivide_source( source )
	
#	var faces: PoolVector3Array = cubes.faces()
#	print( faces.size() )
	
	var mi: MeshInstance = get_node( "MeshInstance" )
	cubes.apply_to_mesh( mi )


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
