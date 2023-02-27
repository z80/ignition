
extends RefFrameNode

export(PackedScene) var CollisionCell = null
var _collision_body: StaticBody = null

var _source_se3: Se3Ref = null

# Called when the node enters the scene tree for the first time.
func _ready():
	_source_se3 = Se3Ref.new()


func _ign_post_process( _delta ):
	if _collision_body == null:
		return
	
	var ref_frame: RefFrameNode = get_parent()
	var se3: Se3Ref = self.relative_to( ref_frame )
	var t: Transform = se3.transform
	_collision_body.transform = t
	
#	# Validation.
#	se3 = self.get_se3()
#	se3 = se3.mul( _source_se3 )
#	print( "validation: q: ", se3.q, "; r: ", se3.r )


func _get_collision_body():
	# The parent should be RefFramePhysics.
	var ref_frame_physics: RefFrameNode = get_parent()
	if ref_frame_physics == null:
		return null
	
	# Here it should be for ref frame physical space.
	if _collision_body == null:
		_collision_body = CollisionCell.instance()
		var phys_env: Node = ref_frame_physics.get_physics_environment()
		phys_env.add_physics_body( _collision_body )
	
	return _collision_body



func build_surface_prepare( source_se3: Se3Ref, view_point_se3: Se3Ref, node_size_strategy: VolumeNodeSizeStrategyGd, source_surface: Resource ):
	var collision_body: StaticBody = _get_collision_body()
	if collision_body == null:
		return null
	
	var args: BuildArgs = BuildArgs.new()
	args.source_se3         = Se3Ref.new()
	args.source_se3.copy_from( source_se3 )
	_source_se3.copy_from( args.source_se3 )
	args.view_point_se3 = Se3Ref.new()
	args.view_point_se3.copy_from( view_point_se3 )
	args.node_size_strategy   = node_size_strategy
	args.surface_source = source_surface
	
	return args





func build_surface_process( args ):
	var node: BoundingNodeGd = args.node
	var source_se3: Se3Ref = args.source_se3
	var view_point_se3: Se3Ref = args.view_point_se3
	var node_size_strategy: VolumeNodeSizeStrategyGd = args.node_size_strategy
	var source_surface: VolumeSourceGd = args.surface_source.get_source()

	var voxel_surface: MarchingCubesDualGd = MarchingCubesDualGd.new()
	voxel_surface.max_nodes_qty   = 20000000
	voxel_surface.split_precision = 0.01
	
	var _step: float = voxel_surface.init_min_step( source_surface )
	var _ok: bool = voxel_surface.subdivide_source( node, source_surface, null )
	args.ok = _ok
	var _qty: int = voxel_surface.precompute_scaled_values( source_se3, 0, 1.0 )
	args.ok = args.ok and (_qty > 0)
	
	args.voxel_surface = voxel_surface
	return args


func build_surface_finished( args ):
	var voxel_surface: MarchingCubesDualGd = args.voxel_surface
	var qty: int = voxel_surface.get_nodes_qty()
	#print( "surface done, nodes qty: ", qty )
	
	var static_body: StaticBody = _get_collision_body()
	var collision_shape: CollisionShape = static_body.get_collision_shape()
	var shape: ConcavePolygonShape = ConcavePolygonShape.new()
	#voxel_surface.apply_to_mesh_only( _visual.surface )
	#voxel_surface.apply_to_mesh_only_wireframe( _visual.surface )
	var _ok: bool = voxel_surface.apply_to_collision_shape( shape )
	collision_shape.shape = shape
	
	# In ref frame physics it is in the origin.
	var se3: Se3Ref = Se3Ref.new()
	self.set_se3( se3 )







class BuildArgs:
	var ok: bool
	var node: BoundingNodeGd
	var source_se3: Se3Ref
	var view_point_se3: Se3Ref
	var node_size_strategy: VolumeNodeSizeStrategyGd
	var surface_source: Resource
	var voxel_surface: MarchingCubesDualGd


