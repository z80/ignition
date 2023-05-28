extends Node3D


# Called when the node enters the scene tree for the first time.
func _ready():
	var viewport: SubViewport = get_node( "SubViewport" )
	var plane: MeshInstance3D = get_node( "Plane" )
	var material: StandardMaterial3D = plane.get_surface_override_material(0)
	var tex: ViewportTexture = material.albedo_texture
	var path: NodePath = viewport.get_path()
	tex.viewport_path = path
	material.albedo_texture = tex


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
