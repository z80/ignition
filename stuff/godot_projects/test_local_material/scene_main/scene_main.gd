extends Spatial

export(Material) var material = null


# Called when the node enters the scene tree for the first time.
func _ready():
	var a: MeshInstance = get_node( "BodyA" )
	var b: MeshInstance = get_node( "BodyB" )
	
	# MAterial is a ShaderMaterial with "local to scene" being true.
	var sm: ShaderMaterial = material.duplicate()
	a.material_override = sm
	
	sm = material.duplicate()
	b.material_override = sm
	
	sm = a.material_override
	sm.resource_local_to_scene = true
	sm.setup_local_to_scene()
	# Supposed to make it red.
	sm.set_shader_param( "albedo", Color( 1.0, 0.0, 0.0, 0.0 ) )
	a.material_override = sm
	
	sm = b.material_override
	sm.resource_local_to_scene = true
	sm.setup_local_to_scene()
	# Supposed to make it blue.
	sm.set_shader_param( "albedo", Color( 0.0, 0.0, 1.0, 0.0 ) )
	b.material_override = sm


