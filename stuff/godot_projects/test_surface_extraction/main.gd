extends Node3D

var create_surface: bool = true
var triangles: Array = []

# Called when the node enters the scene tree for the first time.
func _ready():
	pass

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _physics_process(delta):
	if create_surface:
		var state: PhysicsDirectSpaceState3D = get_world_3d().direct_space_state
		var v: VolMcGd = VolMcGd.new()
		triangles = v.build_surface( Vector3(-0.0, -0.2, -0.0), state, 0.5, 1 )
		create_surface = false
		
		var im: ImmediateMesh = ImmediateMesh.new()
		im.surface_begin(Mesh.PRIMITIVE_LINES)
		
		var qty: int = triangles.size()
		for i in range(0, qty, 3):
			var pt_a: Vector3 = triangles[i]
			var pt_b: Vector3 = triangles[i+1]
			var pt_c: Vector3 = triangles[i+2]
			im.surface_add_vertex( pt_a )
			im.surface_add_vertex( pt_b )
			im.surface_add_vertex( pt_b )
			im.surface_add_vertex( pt_c )
			im.surface_add_vertex( pt_c )
			im.surface_add_vertex( pt_a )
		
		im.surface_end()
		
		var mesh_inst: MeshInstance3D = get_node( "MeshInstance3D" )
		mesh_inst.mesh = im


