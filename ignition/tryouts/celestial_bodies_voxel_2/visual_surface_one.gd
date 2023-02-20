
extends RefFrameNode

export(PackedScene) var VisualCell = null

var _visual: Spatial = null

var _created_instances: Array = []

var _source_se3: Se3Ref = null

# Called when the node enters the scene tree for the first time.
func _ready():
	_source_se3 = Se3Ref.new()
	call_deferred( "_create_visual" )


func _ign_post_process( _delta ):
	var cam: RefFrameNode = RootScene.ref_frame_root.player_camera
	var se3: Se3Ref = self.relative_to( cam )
	var t: Transform = se3.transform
	_visual.transform = t
	
#	# Validation.
#	se3 = self.get_se3()
#	se3 = se3.mul( _source_se3 )
#	print( "validation: q: ", se3.q, "; r: ", se3.r )


func _create_visual():
	var root: Spatial = RootScene.get_visual_layer_near()
	#var root: Spatial = RootScene.get_visual_layer_space()
	_visual = VisualCell.instance()
	root.add_child( _visual )


func build_surface_prepare( source_se3: Se3Ref, view_point_se3: Se3Ref, node_size_strategy: VolumeNodeSizeStrategyGd, source_surface: Resource, source_liquid: Resource ):
	_visual.visible = false
	
	self.set_se3( view_point_se3 )
	#self.set_se3( center_se3 )
	
	var args: BuildArgs = BuildArgs.new()
	args.source_se3         = Se3Ref.new()
	args.source_se3.copy_from( source_se3 )
	_source_se3.copy_from( args.source_se3 )
	args.view_point_se3 = Se3Ref.new()
	args.view_point_se3.copy_from( view_point_se3 )
	args.node_size_strategy   = node_size_strategy
	args.surface_source_solid = source_surface
	if source_liquid != null:
		args.surface_source_liquid = source_liquid
	else:
		args.surface_source_liquid = null
	
	return args





func build_surface_process( args ):
	var node: BoundingNodeGd = args.node
	var source_se3: Se3Ref = args.source_se3
	var view_point_se3: Se3Ref = args.view_point_se3
	var node_size_strategy: VolumeNodeSizeStrategyGd = args.node_size_strategy
	var source_surface: VolumeSourceGd = args.surface_source_solid.get_source()
	var source_liquid: VolumeSourceGd
	if args.surface_source_liquid != null:
		source_liquid = args.surface_source_liquid.get_source()
	else:
		source_liquid = null

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
	#voxel_surface.apply_to_mesh_only( _visual.surface )
	voxel_surface.apply_to_mesh_only_wireframe( _visual.surface )
	
	var surface_source_solid: Resource = args.surface_source_solid
	#_visual.surface.material_override = surface_source_solid.materials[0]
	_visual.surface.material_override = surface_source_solid.override_material
	
	_visual.visible = args.ok





func _populate_foliage( volume_surface: MarchingCubesDualGd, bounding_node: BoundingNodeGd, foliage_sources: Array ):
	var s: String = bounding_node.hash()
	var rand: IgnRandomGd = IgnRandomGd.new()
	rand.seed = s
	
	var center: Vector3 = bounding_node.center_vector()
	var sz: float       = bounding_node.node_size()
	
	var created_instances: Array = []
	
	for foliage_source in foliage_sources:
	
		var dist: float = foliage_source.min_distance
		var v: float  = sz / dist
		v = v * v / 2
		var qty: int = int(v)
		
		for i in range(qty):
			var dr: Vector3 = rand.random_vector( sz )
			var c: Vector3 = dr + center
			
			var ret: Array = volume_surface.intersect_with_segment( c, Vector3.ZERO )
			
			var intersects: bool = ret[0]
			if ( not intersects ):
				continue
			
			var at: Vector3   = ret[1]
			var norm: Vector3 = ret[2]
			
			var se3: Se3Ref = volume_surface.se3_in_point( at )
			var relative_se3: Se3Ref = volume_surface.relative_to_se3( self )
			var local_se3: Se3Ref = relative_se3.mul( se3 )
			var local_norm: Vector3 = relative_se3.q.xform( norm )
			
			var p: float = foliage_source.probability( se3, norm )
			var rand_p: float = rand.floating_point_closed()
			var create: bool = (rand_p < p)
			if not create:
				continue
			
			var instance: RefFrameNode = foliage_source.create( volume_surface, local_se3, local_norm, rand )
			instance.place( self, se3 )
			#print( "created ", instance )
			created_instances.push_back( instance )
	
	_created_instances = created_instances


func _cleanup_foliage():
	for inst in _created_instances:
		inst.queue_free()




class BuildArgs:
	var ok: bool
	var node: BoundingNodeGd
	var source_se3: Se3Ref
	var view_point_se3: Se3Ref
	var node_size_strategy: VolumeNodeSizeStrategyGd
	var surface_source_solid: Resource
	var surface_source_liquid: Resource
	var voxel_surface: MarchingCubesDualGd


