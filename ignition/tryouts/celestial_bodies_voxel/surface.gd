
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
	


func update( source_se3: Se3Ref, scaler: DistanceScalerBaseRef, synchronous: bool ):
	var dimensions: float      = surface_source.source_dimensions
	var source: VolumeSourceGd = surface_source.source
	
	self.transform = Transform.IDENTITY
	
	if synchronous:
		var ret: Array = _update_worker( source_se3, dimensions, source, scaler )
		_update_finished( ret )
	
	else:
		WorkersPool.start_with_args( self, "_update_worker", "_update_finished", [source_se3, dimensions, source, scaler] )


func _update_worker( source_se3: Se3Ref, dimensions: float, source: VolumeSourceGd, scaler: DistanceScalerBaseRef ):
	_voxel_surface.max_nodes_qty   = 20000000
	_voxel_surface.source_se3      = source_se3
	_voxel_surface.split_precision = 0.01
	var ok: bool = _voxel_surface.subdivide_source( dimensions, source, scaler )
	var ret: Array = [ok, source_se3, scaler]
	return ret


func _update_finished( data: Array ):
	var ok: bool = data[0]
	if not ok:
		return
	
	var source_se3: Se3Ref = data[1]
	var scaler: DistanceScalerBaseRef = data[2]
	apply_meshes( source_se3, scaler )
	




func apply_meshes( source_se3: Se3Ref, scaler: DistanceScalerBaseRef ):
	# Once again apply source_se3
	_voxel_surface.source_se3      = source_se3
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
		var mi: MeshInstance = meshes[i]
		mi.visible = true
		_voxel_surface.apply_to_mesh( material_ind, mi, scaler )
		mi.material_override = materials[material_ind]

# This is used by FoliageSource
func get_voxel_surface():
	return _voxel_surface


func get_surface_radius():
	var r: float = surface_source.source_dimensions
	return r




