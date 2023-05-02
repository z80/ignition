extends Node3D

const EPS: float = 0.0001

const PROBE_DIST: float = 100.0

var Ortho = preload( "res://physics/utils/orthogonalize.gd" )
var _skeleton: Skeleton3D = null
var _forward_vector: Vector3 = Vector3.FORWARD

var _global_bone_local_positions: Array = []
var _global_to_local_bone: Array        = []
var _global_bone_to_global: Transform3D


# Called when the node enters the scene tree for the first time.
func _ready():
	_compute_forward_vector()
	_compute_bone_transforms()


func apply( v: Vector3, broad_tree: BroadTreeGd, meshes: Array ):
	# Orient along velocity.
	var adjustment_t: Transform3D = _compute_orientation( v )
	var own_t: Transform3D = self.transform
	own_t = adjustment_t * own_t
	self.transform = own_t
	
	var finish: Vector3 = own_t.origin
	
	var global_bone_to_global: Transform3D = own_t * _global_bone_to_global
	var global_to_global_bone: Transform3D = global_bone_to_global.inverse()
	
	var s: Skeleton3D = _get_skeleton()
	var qty: int = _global_bone_local_positions.size()
	
	var global_poses: Array = []
	
	for i in range(qty):
		var at: Vector3 = _global_bone_local_positions[i]
		var start: Vector3 = global_bone_to_global * (PROBE_DIST * at)
		var ret: Array = broad_tree.intersects_segment_face( start, finish, null )
		var intersects: bool = ret[0]
		
		if intersects:
			var mesh_gd: OctreeMeshGd = ret[6]
			var in_the_list: bool = meshes.has( mesh_gd )
			intersects = intersects and in_the_list
		
		var bone_t: Transform3D = s.get_bone_pose( i )
		if not intersects:
			bone_t.origin = at
		
		else:
			var bone_at: Vector3 = ret[2]
			var dr: Vector3 = bone_at - finish
			at = finish + dr * 1.5
			var to_local_t: Transform3D = _global_to_local_bone[i]
			to_local_t = to_local_t * global_to_global_bone
			at = to_local_t * (at)
			bone_t.origin = at
		
		s.set_bone_pose( i, bone_t )
		
		var gt: Vector3 = s.get_bone_global_pose( i ).origin
		global_poses.push_back( gt )
	
	#print( "from: ", _global_bone_local_positions )
	#print( "to:   ", global_poses )



func _get_skeleton():
	if _skeleton == null:
		_skeleton = get_node( "Armature/Skeleton3D" )
	
	return _skeleton


func _compute_forward_vector():
	var s: Skeleton3D = _get_skeleton()
	var ind: int = s.find_bone( "Forward" )
	
	var bone_t: Transform3D = s.get_bone_global_pose( ind )
	var skeleton_t: Transform3D = s.transform
	var armature_t: Transform3D = get_node( "Armature" ).transform
	var total_bone_t: Transform3D = armature_t * skeleton_t * bone_t
	var at: Vector3 = total_bone_t.origin
	_forward_vector = at.normalized()


func _compute_bone_transforms():
	var s: Skeleton3D = _get_skeleton()
	var qty: int = s.get_bone_count()

	var skeleton_t: Transform3D = s.transform
	var armature_t: Transform3D = get_node( "Armature" ).transform
	var total_t: Transform3D = armature_t * skeleton_t

	_global_bone_to_global = total_t
	_global_bone_local_positions = []
	_global_to_local_bone = []
	for i in range(qty):
		var bone_t: Transform3D = s.get_bone_global_pose( i )
		_global_bone_local_positions.push_back( bone_t.origin )
		
		var rest_t: Transform3D   = s.get_bone_rest( i )
		var custom_t: Transform3D = s.get_bone_custom_pose( i )
		var bone_to_global: Transform3D = total_t * rest_t * custom_t
		var global_to_bone: Transform3D = bone_to_global.inverse()
		_global_to_local_bone.push_back( global_to_bone )


func _compute_orientation(v: Vector3):
	var t: Transform3D = self.transform
	var forward: Vector3 = t.basis * (_forward_vector)
	# compute rotation converting "forward" to "v".
	var a: Vector3 = forward.cross( v.normalized() )
	var si: float = a.length()
	var q: Quaternion
	
	if si < EPS:
		q = Quaternion.IDENTITY
	
	else:
		var angle_2: float = asin(si) * 0.5
		var si2: float = sin(angle_2)
		var co2: float = cos(angle_2)
		a = a.normalized()
		q.x = a.x*si2
		q.y = a.y*si2
		q.z = a.z*si2
		q.w = co2
	
	var b: Basis = Basis(q)
	var tr: Transform3D = Transform3D( b, Vector3.ZERO )
	
	return tr

