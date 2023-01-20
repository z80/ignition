
extends RefFrameNode

export(PackedScene) var VisualCell = null

var _visual: Spatial = null

# Called when the node enters the scene tree for the first time.
func _ready():
	_create_visual()


func _ign_post_process( _delta ):
	var cam: RefFrameNode = RootScene.ref_frame_root.player_camera
	var se3: Se3Ref = self.relative_to( cam )
	var t: Transform = se3.transform()
	_visual.transform = t


func _create_visual():
	var root: Spatial = RootScene.get_visual_layer_near()
	_visual = VisualCell.instance()
	root.add_child( _visual )


func build_surface_prepare( view_point_se3: Se3Ref, node_size_strategy: VolumeNodeSizeStrategyGd, source_surface: Resource, source_liquid: Resource ):
	self.set_se3( view_point_se3 )
	var args: BuildArgs = BuildArgs.new()
	args.source_se3         = Se3Ref.new()
	args.source_se3.copy_from( view_point_se3 )
	args.node_size_strategy = node_size_strategy
	args.surface_source_solid     = source_surface
	if source_liquid != null:
		args.surface_source_liquid = source_liquid
	else:
		args.surface_source_liquid = null

	



func build_surface_process( args ):
	var source_se3: Se3Ref = args.source_se3
	var node_size_strategy: VolumeNodeSizeStrategyGd = args.node_size_strategy
	var source_surface: VolumeSourceGd = args.surface_source_solid.get_source()
	var source_liquid: VolumeSourceGd  = args.surface_source_liquid.get_source()

	var voxel_surface: MarchingCubesDualGd = MarchingCubesDualGd.new()
	voxel_surface.max_nodes_qty   = 20000000
	voxel_surface.split_precision = 0.01
	
	var ok: bool = voxel_surface.subdivide_source_all( source_surface, null )
	voxel_surface.precompute_scaled_values( source_se3, source_se3, 0, 1.0 )
	
	args.voxel_surface = voxel_surface
	return args


func build_finished( args ):
	var voxel_surface: MarchingCubesDualGd = args.voxel_surface
	var qty: int = voxel_surface.get_nodes_qty()
	print( "rebuild done, nodes qty: ", qty )
	voxel_surface.apply_to_mesh_only( _visual.surface )
	
	var surface_source_solid: Resource = args.surface_source_solid
	_visual.surface.material_override = surface_source_solid.materials[0]





class BuildArgs:
	var source_se3: Se3Ref
	var node_size_strategy: VolumeNodeSizeStrategyGd
	var surface_source_solid: Resource
	var surface_source_liquid: Resource
	var volume_surface: MarchingCubesDualGd


