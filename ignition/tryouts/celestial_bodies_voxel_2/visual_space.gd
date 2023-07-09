
extends Node3D

@export var layer_config: Resource = null
@export var surface_source: Resource = null

var solid: MeshInstance3D  = null
var liquid: MeshInstance3D = null
var atmosphere: Node3D  = null

var _rebuild_strategy: MarchingCubesRebuildStrategyGd = null
var _node_size_strategy: VolumeNodeSizeStrategyGd = null
var _scale_dist_ratio: ScaleDistanceRatioGd = null

var _center_se3: Se3Ref = null

var _is_ready: bool = false
var _running: bool = false
var _requested_rebuild: bool = false

# Called when the node enters the scene tree for the first time.
func _ready():
	var root: Node3D = get_node( "Visual" )
	solid  = root.solid
	liquid = root.liquid
	atmosphere = get_node( "Atmosphere" )
	
	_center_se3 = Se3Ref.new()
	
	_is_ready = false
	
	_initialize_scale_distance_ratio()
	_initialize_strategies()


func _initialize_scale_distance_ratio():
	_scale_dist_ratio = ScaleDistanceRatioGd.new()
	_scale_dist_ratio.max_distance = layer_config.event_horizon


func _initialize_strategies():
	var radius: float      = surface_source.bounding_radius
	
	var focus_depth: float = radius * layer_config.min_relative_focal_depth
	var rebuild_dist: float = radius * layer_config.relative_rebuild_dist
	var max_level: int = layer_config.max_level
	
	_rebuild_strategy = MarchingCubesRebuildStrategyGd.new()
	_rebuild_strategy.radius = radius
	_rebuild_strategy.height = focus_depth
	_rebuild_strategy.rebuild_dist = rebuild_dist
	
	_node_size_strategy = VolumeNodeSizeStrategyGd.new()
	_node_size_strategy.radius = radius
	_node_size_strategy.max_level = max_level


func update_source_se3( rotation: RefFrameRotationNode, source_se3: Se3Ref, view_point_se3: Se3Ref ):
	var rebuild_needed: bool = _rebuild_strategy.need_rebuild( view_point_se3 )
	if _running:
		if rebuild_needed:
			_requested_rebuild = true
	
	else:
		rebuild_needed = rebuild_needed or _requested_rebuild
		if rebuild_needed:
			_rebuild_start( view_point_se3 )
	
	var scale: float = 1.0 / layer_config.scale_divider
	var t: Transform3D = _scale_dist_ratio.compute_transform( source_se3, scale )
	solid.transform = t
	liquid.transform = t
	
	atmosphere.update( rotation )



func _rebuild_start( view_point_se3: Se3Ref ):
	_running = true
	_requested_rebuild = false
	
	_node_size_strategy.focal_point = _rebuild_strategy.get_focal_point_rebuild()
	var source_solid: VolumeSourceGd  = surface_source.get_source_solid()
	var source_liquid: VolumeSourceGd = surface_source.get_source_liquid()
	
	var common_point: Vector3 = _node_size_strategy.focal_point
	
	var scale: float = 1.0 / layer_config.scale_divider
	var args: Dictionary = { "source_solid": source_solid, "source_liquid": source_liquid, "scale": scale, "common_point": common_point }
	
	WorkersPool.push_back_with_arg( self, "_rebuild_process", "_rebuild_finished", args )
	#var ret = _rebuild_process( args )
	#_rebuild_finished( ret )


func _rebuild_process( args ):
	var source_solid: VolumeSourceGd  = args.source_solid
	var source_liquid: VolumeSourceGd = args.source_liquid
	var scale: float = args.scale
	
	var voxel_surface_solid: MarchingCubesDualGd = MarchingCubesDualGd.new()
	voxel_surface_solid.max_nodes_qty   = 20000000
	voxel_surface_solid.split_precision = 0.01
	
	var common_point: Vector3 = args.common_point
	
	var _ok: bool = voxel_surface_solid.subdivide_source_all( source_solid, _node_size_strategy )
	voxel_surface_solid.precompute_scaled_values( _center_se3, 0, scale, common_point )

	var ret: Dictionary = { "voxel_surface_solid": voxel_surface_solid, "common_point": common_point }

	var voxel_surface_liquid: MarchingCubesDualGd
	if source_liquid != null:
		voxel_surface_liquid = MarchingCubesDualGd.new()
		voxel_surface_liquid.max_nodes_qty   = 20000000
		voxel_surface_liquid.split_precision = 0.01
		_ok = voxel_surface_liquid.subdivide_source_all( source_liquid, _node_size_strategy )
		voxel_surface_liquid.precompute_scaled_values( _center_se3, 0, scale, common_point )
		ret["voxel_surface_liquid"] = voxel_surface_liquid
	
	else:
		ret["voxel_surface_liquid"] = null
	
	return ret


func _rebuild_finished( args ):
	_is_ready = true
	_running = false
	
	var voxel_surface_solid: MarchingCubesDualGd = args.voxel_surface_solid
	var voxel_surface_liquid: MarchingCubesDualGd = args.voxel_surface_liquid
	var qty: int = voxel_surface_solid.get_nodes_qty()
	print( "rebuild done, nodes qty: ", qty )
	#voxel_surface_solid.apply_to_mesh_only( solid )
	voxel_surface_solid.apply_to_mesh_only_wireframe( solid )
	
	var sm: ShaderMaterial = solid.material_override
	if sm == null:
		sm = surface_source.materials_solid[0].duplicate()
	
	var pt: Vector3 = args.common_point
	sm.set_shader_parameter( "common_point", pt )
	sm.set_shader_parameter( "to_planet_rf", Basis.IDENTITY )
	solid.material_override = sm
	
	
	if voxel_surface_liquid != null:
		#voxel_surface_liquid.apply_to_mesh_only( liquid )
		voxel_surface_solid.apply_to_mesh_only_wireframe( solid )
		
		sm = liquid.material_override
		if sm == null:
			sm = surface_source.materials_liquid[0].duplicate()
		
		sm.set_shader_parameter( "common_point", pt )
		sm.set_shader_parameter( "to_planet_rf", Basis.IDENTITY )
		liquid.material_override = sm




func get_surface_source():
	return surface_source




