extends Spatial


#var source: VolumeSourceScriptGd     = null
#var material: MaterialSourceScriptGd = null
#var cubes: MarchingCubesGd = null

var meshes: Array = []

# Called when the node enters the scene tree for the first time.
func _ready():
	return
	meshes = [ get_node("Mesh_0" ), get_node("Mesh_1") ]
	source = VolumeSourceScriptGd.new()
	var script: Resource = preload( "res://volume_source_sphere.gd" )
	source.set_script( script )
	
	material = MaterialSourceScriptGd.new()
	script = preload( "res://material_source_sphere.gd" )
	material.set_script( script )
	
	cubes = MarchingCubesGd.new()
	cubes.max_nodes_qty = 20000
	cubes.subdivide_source( source, material )
	
	var material_inds: Array = cubes.materials_used()
	
	for mi in meshes:
		mi.visible = false
	
	for ind in material_inds:
		var m: MeshInstance = meshes[ind]
		m.visible = true
		cubes.apply_to_mesh( ind, m, null )


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
