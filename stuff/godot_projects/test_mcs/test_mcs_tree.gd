extends Spatial

var source_tree: VolumeSourceTreeGd  = null
var source: VolumeSourceScriptGd     = null
var material: MaterialSourceScriptGd = null
var cubes: MarchingCubesGd = null

var meshes: Array = []

# Called when the node enters the scene tree for the first time.
func _ready():
	meshes = [ get_node("Mesh_0" ), get_node("Mesh_1") ]
	
	source_tree = VolumeSourceTreeGd.new()
	
	source = VolumeSourceScriptGd.new()
	var script: Resource = preload( "res://volume_source_sphere.gd" )
	source.set_script( script )
	source.bounding_radius = 3.5
	
	var se3: Se3Ref = Se3Ref.new()
	se3.r = Vector3.ZERO
	source.se3 = se3
	
	source_tree.add_source( source )



	source = VolumeSourceScriptGd.new()
	script = preload( "res://volume_source_sphere.gd" )
	source.set_script( script )
	source.bounding_radius = 5.0
	source.inverted = false

	se3.r = Vector3( 3.0, 1.5, 0.0 )
	source.se3 = se3

	source_tree.add_source( source )




	
	source = VolumeSourceScriptGd.new()
	script = preload( "res://volume_source_sphere.gd" )
	source.set_script( script )
	source.bounding_radius = 5.0

	se3.r = Vector3( 0.0, 2.5, 0.0 )
	source.se3 = se3
	
	source.inverted = true

	source_tree.add_source( source )



	source_tree.subdivide( 100.0 )


	
	
	
	cubes = MarchingCubesGd.new()
	cubes.max_nodes_qty = 20000
	cubes.subdivide_source( source_tree )
	
	var material_inds: Array = cubes.materials_used()
	
	for mi in meshes:
		mi.visible = false
	
	for ind in material_inds:
		if ind < 0:
			ind = 0
		var m: MeshInstance = meshes[ind]
		m.visible = true
		cubes.apply_to_mesh( ind, m, null )


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
