
extends Spatial

export(Resource) var surface_source = null

## If solid, it generates physical surface 
## for collision processing.
export(bool)     var solid = true

var _surface_meshes: Array = []
var _voxel_surface: MarchingCubesDualGd   = null
#var _physical_surface: MarchingCubesDualGd = null

var _rebuild_tasks_qty: int = 0
var _adjust_tasks_qty: int = 0

# Called when the node enters the scene tree for the first time.
func _ready():
	_surface_meshes = []
	_voxel_surface = MarchingCubesDualGd.new()


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
	


func rebuild_surface( source_se3: Se3Ref, scaler: DistanceScalerBaseRef, synchronous: bool ):
	if _rebuild_tasks_qty != 0:
		return false
	
	var dimensions: float      = surface_source.source_dimensions
	var source: VolumeSourceGd = surface_source.get_source()
	
	self.transform = Transform.IDENTITY
	
	_rebuild_tasks_qty += 1
	if synchronous:
		var ret: Array = _rebuild_surface_worker( source_se3, dimensions, source, scaler )
		_rebuild_surface_worker_finished( ret )
	
	else:
		WorkersPool.start_with_args( self, "_rebuild_surface_worker", "_rebuild_surface_worker_finished", [source_se3, dimensions, source, scaler] )
	
	return true


func _rebuild_surface_worker( source_se3: Se3Ref, dimensions: float, source: VolumeSourceGd, scaler: DistanceScalerBaseRef ):
	_voxel_surface.max_nodes_qty   = 20000000
	_voxel_surface.source_se3      = source_se3
	_voxel_surface.split_precision = 0.01
	var ok: bool = _voxel_surface.subdivide_source( dimensions, source, scaler )
	var ret: Array = [ok, source_se3, scaler]
	return ret


func _rebuild_surface_worker_finished( data: Array ):
	_rebuild_tasks_qty -= 1
	
	var ok: bool = data[0]
	if not ok:
		return
	
	var source_se3: Se3Ref = data[1]
	var scaler: DistanceScalerBaseRef = data[2]
	adjust_view_point( source_se3, scaler )





func adjust_view_point( source_se3: Se3Ref, scaler: DistanceScalerBaseRef ):
	if _adjust_tasks_qty != 0:
		return false
	
	# If succeeded apply to meshes.
	var material_inds: Array = _voxel_surface.materials_used()
	
	var meshes: Array = []
	for ind in material_inds:
		var mi: MeshInstance = _get_surface_mesh( ind )
		meshes.push_back( mi )
	
	for mi in meshes:
		mi.visible = false
	
	var materials: Array = surface_source.materials
	
	var qty: int = material_inds.size()
	for i in range(qty):
		var material_ind: int = material_inds[i]
		if material_ind < 0:
			material_ind = 0
		var mesh_inst: MeshInstance = meshes[i]
		mesh_inst.visible = true
		mesh_inst.material_override = materials[material_ind]
		_adjust_tasks_qty += 1
		if true:
			var args: Array = [source_se3, mesh_inst, material_ind, scaler]
			WorkersPool.start_with_args( self, "_adjust_view_point_worker", "_adjust_view_point_worker_finished", args )
		else:
			_adjust_view_point_worker( source_se3, mesh_inst, material_ind, scaler )
			_adjust_view_point_worker_finished( mesh_inst )
	
	return true




func _adjust_view_point_worker( source_se3: Se3Ref, mesh_inst: MeshInstance, material_ind: int, scaler: DistanceScalerBaseRef ):
	print( "_adjust_view_point_worker entered" )
	_voxel_surface.source_se3 = source_se3
	_voxel_surface.precompute_scaled_values( material_ind, scaler )
	print( "_adjust_view_point_worker left" )
	return mesh_inst



func _adjust_view_point_worker_finished( mesh_inst: MeshInstance ):
	_adjust_tasks_qty -= 1
	_voxel_surface.apply_to_mesh_only( mesh_inst )




func get_root_se3( source_se3: Se3Ref, scaler: DistanceScalerBaseRef ):
	_voxel_surface.source_se3 = source_se3
	var t: Transform = _voxel_surface.mesh_transform( scaler )
	return t



# This is used by FoliageSource
func get_voxel_surface():
	return _voxel_surface


func get_surface_radius():
	var r: float = surface_source.source_dimensions
	return r




