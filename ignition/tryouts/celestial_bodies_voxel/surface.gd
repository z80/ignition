
extends Spatial

export(Resource) var surface_source = null

## If solid, it generates physical surface 
## for collision processing.
export(bool)     var solid = true

var _material_inds: Array = []
var _surface_meshes: Array = []
var _voxel_surface: MarchingCubesDualGd   = null
#var _physical_surface: MarchingCubesDualGd = null

# Here we store the point in source ref. frame with respect to which all 
# triangles are computed.
var _local_point_se3: Se3Ref = null
var _new_local_point_se3: Se3Ref = null

var _mutex: Mutex = null
var _locked_by: String = ""
var _locked: bool = false


# Called when the node enters the scene tree for the first time.
func _ready():
	_mutex = Mutex.new()
	
	_surface_meshes = []
	_voxel_surface = MarchingCubesDualGd.new()
	_voxel_surface.split_precision = 0.001
	
	_local_point_se3 = Se3Ref.new()
	_new_local_point_se3       = Se3Ref.new()


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
	var source_radius: float = get_surface_radius()
	var source: VolumeSourceGd = surface_source.get_source()
	
	_voxel_surface.max_nodes_qty   = 20000000
	_voxel_surface.split_precision = 0.01
	lock()
	var ok: bool = _voxel_surface.subdivide_source_all( source, strategy )
	#print( "material inds: ", _material_inds )
	unlock()
	var ret: Array = [ok, source_se3, scaler]

	return ret


func rebuild_surface_finished( data: Array ):
	pass



func update_material_properties( source_se3: Se3Ref, scaler: DistanceScalerBaseRef ):
	var meshes: Array = _surface_meshes
	var qty: int = meshes.size()
	if qty < 1:
		return
	
	var materials: Array = surface_source.materials
	
	var ref_point_transform: Se3Ref = source_se3.mul( _local_point_se3 )
	
	var s: DistanceScalerRef = scaler
	var plain_dist: float = s.plain_distance
	var log_scale: float  = s.log_scale
	var source_tr: Transform = ref_point_transform.transform
	var scaled_source_se3: Se3Ref = _voxel_surface.compute_source_se3( source_se3, _local_point_se3 )
	var inv_scaled_source_se3: Se3Ref = scaled_source_se3.inverse()
	var inv_scaled_source_tr: Transform = inv_scaled_source_se3.transform
	
	# How much change vertex position to get true displacement with respect to 
	# source origin in origin ref. frame.
	var displacement: Vector3 = _local_point_se3.r
	
	for i in range(qty):
		var mesh_inst: MeshInstance = meshes[i]
		var sm: ShaderMaterial = mesh_inst.material_override as ShaderMaterial
		if sm == null:
			continue
		sm.set_shader_param( "plain_dist", plain_dist )
		sm.set_shader_param( "log_scale", log_scale )
		sm.set_shader_param( "source_tr", source_tr )
		sm.set_shader_param( "inv_scaled_source_tr", inv_scaled_source_tr )
		sm.set_shader_param( "displacement", displacement )


func rescale_surface( source_se3: Se3Ref, local_point_se3: Se3Ref, scaler: DistanceScalerBaseRef ):
	# If succeeded apply to meshes.
	var material_inds: Array = _voxel_surface.materials_used()
	
	var meshes: Array = []
	for ind in material_inds:
		var mi: MeshInstance = _get_surface_mesh( ind )
		meshes.push_back( mi )
	
	for mi in meshes:
		mi.visible = false
	
	# Remember the new reference point.
	_new_local_point_se3.copy_from( local_point_se3 )
	
	var materials: Array = surface_source.materials
	
	var qty: int = material_inds.size()
	for i in range(qty):
		var material_ind: int = material_inds[i]
		if material_ind < 0:
			material_ind = 0
		var mesh_inst: MeshInstance = meshes[i]
		mesh_inst.visible = true
		mesh_inst.material_override = materials[material_ind]
		
		lock()
		#_voxel_surface.precompute_scaled_values( source_se3, local_point_se3, material_ind, 1.0 )
		unlock()
	
	return true


func rescale_surface_finished( local_point_se3: Se3Ref, wireframe: bool = false ):
	# Copy the new reference point.
	_local_point_se3.copy_from( _new_local_point_se3 )
	
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
	
		lock()
		if wireframe:
			_voxel_surface.apply_to_mesh_only_wireframe( mesh_inst )
		else:
			_voxel_surface.apply_to_mesh_only( mesh_inst )
		unlock()








func get_root_se3( source_se3: Se3Ref, scaler: DistanceScalerBaseRef ):
	var t: Transform = _voxel_surface.compute_source_transform( source_se3, _local_point_se3 )
	return t



# This is used by FoliageSource
func get_voxel_surface():
	return _voxel_surface


func get_surface_radius():
	var r: float = surface_source.source_radius
	return r


func get_focus_depth():
	var d: float = surface_source.focus_depth
	return d


func get_identity_distance():
	var d: float = surface_source.identity_distance
	return d


func get_surface_source():
	return surface_source


func get_node_sizes():
	var pairs: Array = surface_source.node_sizes
	var qty: int = pairs.size()
	var distances: Array  = []
	var node_sizes: Array = []
	for i in range(qty):
		var d: Dictionary    = pairs[i]
		var dist: float      = d.distance
		var node_size: float = d.node_size
		distances.push_back( dist )
		node_sizes.push_back( node_size )
	return [ distances, node_sizes ]



func _lock_location():
	var stack: Array = get_stack()
	var stri: String = ""
	for page in stack:
		var fu: String = str( page["function"] )
		var li: String = str( page["line"] )
		var so: String = str( page["source"] )
		if stri.length() > 0:
			stri += "\n"
		stri += so + '[' + li + '[' + fu + ']]'
	return stri


func lock():
#	var ret: int = _mutex.try_lock()
#	if ret != OK:
	#if _locked:
#		var try: String = _lock_location()
#		print( "trying to lock at: ", try )
#		print( "but already locked at: ", _locked_by )
	_mutex.lock()
	
#	_locked_by = _lock_location()
	_locked = true


func unlock():
	_mutex.unlock()
	_locked = false

