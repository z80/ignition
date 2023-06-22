
extends RefFrameNode

@export var CollisionCell: PackedScene = null
@export var VisualCell: PackedScene    = null

var bounding_node: BoundingNodeGd = null

var _collision_body: StaticBody3D      = null
var _visual: Node3D = null

var _created_instances: Array = []


var _source_se3: Se3Ref = null

# Called when the node enters the scene tree for the first time.
func _ready():
	_source_se3 = Se3Ref.new()
	call_deferred( "_create_visual" )

func _create_visual():
	if _visual != null:
		return
	
	var root: Node3D = RootScene.get_visual_layer_near()
	#var root: Spatial = RootScene.get_visual_layer_space()
	_visual = VisualCell.instantiate()
	root.add_child( _visual )


func _ign_post_process( _delta ):
	if _collision_body == null:
		return
	
	# Collision body placement.
	var ref_frame: RefFrameNode = get_parent()
	var se3: Se3Ref = self.relative_to( ref_frame )
	var t: Transform3D = se3.transform
	_collision_body.transform = t
	
	# Visual body placement.
	var cam: RefFrameNode = RootScene.ref_frame_root.player_camera
	se3 = self.relative_to( cam )
	t = se3.transform
	_visual.transform = t



func _get_collision_body():
	# The parent should be RefFramePhysics.
	var ref_frame_physics: RefFrameNode = get_parent()
	if ref_frame_physics == null:
		return null
	
	# Here it should be for ref frame physical space.
	if _collision_body == null:
		_collision_body = CollisionCell.instantiate()
		var phys_env: Node = ref_frame_physics.get_physics_environment()
		phys_env.add_physics_body( _collision_body )
	
	return _collision_body


func remove_surface():
	var static_body: StaticBody3D = _get_collision_body()
	var collision_shape: CollisionShape3D = static_body.get_collision_shape()
	
	# Just empty shape so that there are no polygons to collide with.
	var shape: ConcavePolygonShape3D = ConcavePolygonShape3D.new()
	collision_shape.shape = shape


func clone_surface( other_cell: RefFrameNode ):
	var other_static_body: StaticBody3D = other_cell._get_collision_body()
	var other_collision_shape: CollisionShape3D = other_static_body.get_collision_shape()
	var shape: ConcavePolygonShape3D = other_collision_shape.shape

	var own_static_body: StaticBody3D = _get_collision_body()
	var own_collision_shape: CollisionShape3D = own_static_body.get_collision_shape()
	own_collision_shape.shape = shape



# DEPRECATE
func build_surface_prepare_collision( source_se3: Se3Ref, view_point_se3: Se3Ref, node_size_strategy: VolumeNodeSizeStrategyGd, source_surface: Resource ):
	var collision_body: StaticBody3D = _get_collision_body()
	if collision_body == null:
		return null
	
	var args: BuildArgsCollision = BuildArgsCollision.new()
	args.source_se3         = Se3Ref.new()
	args.source_se3.copy_from( source_se3 )
	_source_se3.copy_from( args.source_se3 )
	args.view_point_se3 = Se3Ref.new()
	args.view_point_se3.copy_from( view_point_se3 )
	args.node_size_strategy   = node_size_strategy
	args.surface_source = source_surface
	
	return args




# DEPRECATE
func build_surface_process_collision( args ):
	bounding_node = args.node
	var source_se3: Se3Ref = args.source_se3
	var view_point_se3: Se3Ref = args.view_point_se3
	var node_size_strategy: VolumeNodeSizeStrategyGd = args.node_size_strategy
	var source_surface: VolumeSourceGd = args.surface_source.get_source_solid()

	var voxel_surface: MarchingCubesDualGd = MarchingCubesDualGd.new()
	voxel_surface.max_nodes_qty   = 20000000
	voxel_surface.split_precision = 0.01
	
	var _step: float = voxel_surface.init_min_step( source_surface )
	var _ok: bool = voxel_surface.subdivide_source( bounding_node, source_surface, null )
	args.ok = _ok
	var _qty: int = voxel_surface.precompute_scaled_values( source_se3, 0, 1.0, Vector3.ZERO )
	args.ok = args.ok and (_qty > 0)
	
	args.voxel_surface = voxel_surface
	return args


# DEPRECATE
func build_surface_finished_collision( args ):
	var voxel_surface: MarchingCubesDualGd = args.voxel_surface
	var qty: int = voxel_surface.get_nodes_qty()
	#print( "surface done, nodes qty: ", qty )
	
	var static_body: StaticBody3D = _get_collision_body()
	var collision_shape: CollisionShape3D = static_body.get_collision_shape()
	var shape: ConcavePolygonShape3D = ConcavePolygonShape3D.new()
	#voxel_surface.apply_to_mesh_only( _visual.surface )
	#voxel_surface.apply_to_mesh_only_wireframe( _visual.surface )
	var _ok: bool = voxel_surface.apply_to_collision_shape( shape )
	collision_shape.shape = shape
	
	# In ref frame physics it is in the origin.
	var se3: Se3Ref = Se3Ref.new()
	self.set_se3( se3 )






# DEPRECATE
class BuildArgsCollision:
	var ok: bool
	var node: BoundingNodeGd
	var source_se3: Se3Ref
	var view_point_se3: Se3Ref
	var node_size_strategy: VolumeNodeSizeStrategyGd
	var surface_source: Resource
	var voxel_surface: MarchingCubesDualGd













func build_surface_prepare( source_se3: Se3Ref, view_point_se3: Se3Ref, node_size_strategy: VolumeNodeSizeStrategyGd, source_surface: Resource, foliage_sources: Array ):
	var collision_body: StaticBody3D = _get_collision_body()
	if collision_body == null:
		return null

	if _visual == null:
		_create_visual()
	_visual.solid.visible  = false
	_visual.liquid.visible = false
	
	_cleanup_foliage()
	
	#view_point_se3.q = Quat.IDENTITY
	#source_se3 = view_point_se3.inverse()
	#self.set_se3( view_point_se3 )
	#self.set_se3( center_se3 )
	
	var args: BuildArgsVisual = BuildArgsVisual.new()
	args.source_se3         = Se3Ref.new()
	args.source_se3.copy_from( source_se3 )
	_source_se3.copy_from( args.source_se3 )
	args.view_point_se3 = Se3Ref.new()
	args.view_point_se3.copy_from( view_point_se3 )
	args.node_size_strategy   = node_size_strategy
	args.surface_source = source_surface
	args.foliage_sources = foliage_sources
	
	return args





func build_surface_process( args ):
	var node: BoundingNodeGd = args.node
	var source_se3: Se3Ref = args.source_se3
	var view_point_se3: Se3Ref = args.view_point_se3
	var node_size_strategy: VolumeNodeSizeStrategyGd = args.node_size_strategy
	var source_solid: VolumeSourceGd = args.surface_source.get_source_solid()
	var source_liquid: VolumeSourceGd = args.surface_source.get_source_liquid()
	
	var voxel_surface_solid: MarchingCubesDualGd = MarchingCubesDualGd.new()
	voxel_surface_solid.max_nodes_qty   = 20000000
	voxel_surface_solid.split_precision = 0.01
	
	var common_point: Vector3 = args.view_point_se3.r
	
	var _step: float = voxel_surface_solid.init_min_step( source_solid )
	var _ok: bool = voxel_surface_solid.subdivide_source( node, source_solid, null )
	args.solid_ok = _ok
	var _qty: int = voxel_surface_solid.precompute_scaled_values( source_se3, 0, 1.0, common_point )
	args.solid_ok = args.solid_ok and (_qty > 0)
	
	args.voxel_surface_solid = voxel_surface_solid
	
	if source_liquid != null:
		var voxel_surface_liquid: MarchingCubesDualGd = MarchingCubesDualGd.new()
		voxel_surface_liquid.max_nodes_qty   = 20000000
		voxel_surface_liquid.split_precision = 0.01
		
		_step = voxel_surface_liquid.init_min_step( source_liquid )
		_ok = voxel_surface_liquid.subdivide_source( node, source_liquid, null )
		args.liquid_ok = _ok
		_qty = voxel_surface_liquid.precompute_scaled_values( source_se3, 0, 1.0, common_point )
		args.liquid_ok = args.liquid_ok and (_qty > 0)
		
		args.voxel_surface_liquid = voxel_surface_liquid
	
	else:
		args.voxel_surface_liquid = null
		args.liquid_ok = false
		
	return args


func build_surface_finished( args ):
	var bounding_node: BoundingNodeGd      = args.node
	var voxel_surface_solid: MarchingCubesDualGd = args.voxel_surface_solid
	var voxel_surface_liquid: MarchingCubesDualGd = args.voxel_surface_liquid
	var foliage_sources: Array = args.foliage_sources
	#var qty_solid: int = voxel_surface_solid.get_nodes_qty()
	var solid_ok: bool = args.solid_ok
	#print( "surface done, nodes qty: ", qty )
	if solid_ok:
		# Apply collision shape.
		var static_body: StaticBody3D = _get_collision_body()
		var collision_shape: CollisionShape3D = static_body.get_collision_shape()
		var shape: ConcavePolygonShape3D = ConcavePolygonShape3D.new()
		#voxel_surface.apply_to_mesh_only( _visual.surface )
		#voxel_surface.apply_to_mesh_only_wireframe( _visual.surface )
		var _ok: bool = voxel_surface_solid.apply_to_collision_shape( shape )
		collision_shape.shape = shape
		
		# Apply visual shape.
		voxel_surface_solid.apply_to_mesh_only( _visual.solid )
		#voxel_surface_solid.apply_to_mesh_only_wireframe( _visual.solid )
		_visual.solid.visible = true
		var surface_source: Resource = args.surface_source
		var sm: ShaderMaterial = _visual.solid.material_override as ShaderMaterial
		if sm == null:
			sm = surface_source.materials_solid[0].duplicate()
		
		var pt: Vector3 = args.view_point_se3.r
		var b: Basis = Basis( args.view_point_se3.q )
		sm.set_shader_parameter( "common_point", pt )
		sm.set_shader_parameter( "to_planet_rf", b )
		_visual.solid.material_override = sm
	
		#_visual.solid.material_override = surface_source.override_material
	
	if voxel_surface_liquid != null:
		#var qty_liquid: int = voxel_surface_liquid.get_nodes_qty()
		var liquid_ok: bool = args.liquid_ok
		if liquid_ok:
			voxel_surface_liquid.apply_to_mesh_only( _visual.liquid )
			_visual.liquid.visible = true
			var surface_source: Resource = args.surface_source
			
			var sm: ShaderMaterial = _visual.liquid.material_override
			if sm == null:
				sm = surface_source.materials_liquid[0].duplicate()
			
			var pt: Vector3 = args.view_point_se3.r
			var b: Basis = Basis( args.view_point_se3.q )
			sm.set_shader_parameter( "common_point", pt )
			sm.set_shader_parameter( "to_planet_rf", b )
			_visual.liquid.material_override = sm
	
	
	_cleanup_foliage()
	_populate_foliage( voxel_surface_solid, bounding_node, foliage_sources )
	






func _populate_foliage( volume_surface: MarchingCubesDualGd, bounding_node: BoundingNodeGd, foliage_sources: Array ):
	#var id: String = bounding_node.get_node_id()
	var s: String = bounding_node.get_hash()
	#if id == '64,-64,-64,32':
	#	print( "id: ", id, "; hash: ", s )
	var rand: IgnRandomGd = IgnRandomGd.new()
	rand.seed = s
	
	# Direct parent is always a ref. frame.
	var ref_frame: RefFrameNode = get_parent()
	# Ref. frame's parent should be the roation when it is on the surface.
	var rotation: RefFrameRotationNode = ref_frame.get_parent()
	
	var center_se3: Se3Ref = bounding_node.get_center( volume_surface )
	var center: Vector3 = center_se3.r
	var sz: float       = bounding_node.get_size( volume_surface )
	
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
			
			var se3: Se3Ref = volume_surface.se3_in_point( at, null )
			var local_norm: Vector3 = se3.q.inverse() * (norm)
			
			var p: float = foliage_source.probability( se3, norm )
			var rand_p: float = rand.floating_point_closed()
			var create: bool = (rand_p < p)
			if not create:
				continue
			
			var instance: RefFrameNode = foliage_source.create( se3, local_norm, rand )
			instance.place( rotation, se3 )
			#print( "created ", instance )
			created_instances.push_back( instance )
	
	_created_instances = created_instances


func _cleanup_foliage():
	for inst in _created_instances:
		inst.queue_free()
	_created_instances.clear()




class BuildArgsVisual:
	var solid_ok: bool
	var liquid_ok: bool
	var node: BoundingNodeGd
	var source_se3: Se3Ref
	var view_point_se3: Se3Ref
	var node_size_strategy: VolumeNodeSizeStrategyGd
	var surface_source: Resource
	var voxel_surface_solid: MarchingCubesDualGd
	var voxel_surface_liquid: MarchingCubesDualGd
	var foliage_sources: Array


