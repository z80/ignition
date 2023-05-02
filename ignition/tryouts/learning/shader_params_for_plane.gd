@tool

extends MeshInstance3D


@export var height_scale: float = 0.5: set = set_height_scale




# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	set_height_scale( height_scale )

func set_height_scale( h: float ):
	height_scale = h
	var m = self.mesh.surface_get_material( 0 )
	if m:
		m.set_shader_parameter( "height_scale", height_scale )
