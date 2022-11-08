
extends Spatial

export(Resource) var surface_source = null

## If solid, it generates physical surface 
## for collision processing.
export(bool)     var solid = true

var _surface_meshes: Array = []
var _voxel_surface: MarchingCubesDualGd   = null
#var _physical_surface: MarchingCubesDualGd = null

# Called when the node enters the scene tree for the first time.
func _ready():
	_surface_meshes = []
	_voxel_surface = MarchingCubesDualGd.new()
	_voxel_surface.split_precision = 0.001


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass



func _get_surface_mesh( ind: int ):
	var qty: int = _surface_meshes.size()
	while ind >= qty:
		var m: MeshInstance = MeshInstance.new()
		m.name = "Mesh_" + str(qty)
		self.add_child( m )
		_surface_meshes.push_back( m )
		qty += 1
	
	var ret: MeshInstance = _surface_meshes[ind]
	return ret
	


func rebuild_surface( source_se3: Se3Ref, strategy: VolumeNodeSizeStrategyGd, scaler: DistanceScalerBaseRef ):
	var source_radius: float   = surface_source.source_radius
	var source: VolumeSourceGd = surface_source.get_source()
	
	var ret: Array = _rebuild_surface_worker( source_se3, strategy, source, scaler )
	
	return ret


func rebuild_surface_finished( data: Array ):
	_rebuild_surface_worker_finished( data )


func _rebuild_surface_worker( source_se3: Se3Ref, strategy: VolumeNodeSizeStrategyGd, source: VolumeSourceGd, scaler: DistanceScalerBaseRef ):
	_voxel_surface.max_nodes_qty   = 20000000
	_voxel_surface.source_se3      = source_se3
	_voxel_surface.split_precision = 0.01
	var source_radius: float = get_surface_radius()
	var ok: bool = _voxel_surface.subdivide_source( source_radius, source, strategy )
	var ret: Array = [ok, source_se3, scaler]
	return ret


func _rebuild_surface_worker_finished( data: Array ):
	pass





func rescale_surface( source_se3: Se3Ref, scaler: DistanceScalerBaseRef ):
	# If succeeded apply to meshes.
	var material_inds: Array = _voxel_surface.materials_used()
	
	var meshes: Array = []
	for ind in material_inds:
		var mi: MeshInstance = _get_surface_mesh( ind )
		meshes.push_back( mi )
	
	for mi in meshes:
		mi.visible = false
	
	
	var materials: Array = surface_source.materials
	
#	var s: DistanceScalerRef = scaler
#	var plain_dist: float = s.plain_distance
#	var log_scale: float  = s.log_scale
#	var source_tr: Transform = source_se3.transform
#	_voxel_surface.mesh_transform( scaler )
#	for m in materials:
#		var sm: ShaderMaterial = m
#		sm.set_shader_param( "plain_dist", plain_dist )
#		sm.set_shader_param( "log_scale", log_scale )
#		sm.set_shader_param( "source_tr", source_tr )
#		sm.set_shader_param( "inv_scaled_source_tr", inv_scaled_source_tr )

	
	var qty: int = material_inds.size()
	for i in range(qty):
		var material_ind: int = material_inds[i]
		if material_ind < 0:
			material_ind = 0
		var mesh_inst: MeshInstance = meshes[i]
		mesh_inst.visible = true
		mesh_inst.material_override = materials[material_ind]

		_rescale_surface_worker( source_se3, mesh_inst, material_ind, scaler )
	
	return true


func rescale_surface_finished():
	var material_inds: Array = _voxel_surface.materials_used()

	var meshes: Array = []
	for ind in material_inds:
		var mi: MeshInstance = _get_surface_mesh( ind )
		meshes.push_back( mi )

	var qty: int = material_inds.size()
	for i in range(qty):
		var material_ind: int = material_inds[i]
		if material_ind < 0:
			material_ind = 0
		var mesh_inst: MeshInstance = meshes[i]
		_rescale_surface_worker_finished( mesh_inst )



func _rescale_surface_worker( source_se3: Se3Ref, mesh_inst: MeshInstance, material_ind: int, scaler: DistanceScalerBaseRef ):
	print( "_rescale_surface_worker entered" )
	_voxel_surface.source_se3 = source_se3
	_voxel_surface.precompute_scaled_values( material_ind, scaler )
	print( "_rescale_surface_worker left" )
	return mesh_inst



func _rescale_surface_worker_finished( mesh_inst: MeshInstance ):
	_voxel_surface.apply_to_mesh_only( mesh_inst )




func get_root_se3( source_se3: Se3Ref, scaler: DistanceScalerBaseRef ):
	var t: Transform = _voxel_surface.compute_source_transform( source_se3, scaler )
	return t



# This is used by FoliageSource
func get_voxel_surface():
	return _voxel_surface


func get_surface_radius():
	var r: float = surface_source.source_radius
	return r




