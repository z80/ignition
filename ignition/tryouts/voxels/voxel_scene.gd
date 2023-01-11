extends Spatial


export(Resource) var source_resource = null

var _src: VolumeSourceScriptGd = null
var _voxels: MarchingCubesDualGd = null


# Called when the node enters the scene tree for the first time.
func _ready():
	_src = source_resource.get_source()
	_voxels = MarchingCubesDualGd.new()
	var _min_step: float = _voxels.init_min_step( _src )
	var se3: Se3Ref = Se3Ref.new()
	var wanted_size: float = 30.0
	
	var bounding_node: BoundingNodeGd = _voxels.create_bounding_node( se3, wanted_size )
	var ok: bool = _voxels.subdivide_source( bounding_node, _src )
	var qty: int = _voxels.get_nodes_qty()
	
	_voxels.precompute_scaled_values( se3, se3, 0, null )
	var mesh_inst: MeshInstance = get_node( "Mesh" )
	_voxels.apply_to_mesh_only_wireframe( mesh_inst )

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
