
extends Node3D
class_name ShockStripe

const EPS: float = 0.0001

const PROBE_DIST: float = 100.0

@export var random_angle: bool = false
@export var angle: float = 0.0

var _material: ShaderMaterial = null

var Ortho = preload( "res://physics/utils/orthogonalize.gd" )
var _skeleton: Skeleton3D = null
var _forward_vector: Vector3 = Vector3.FORWARD

var _bone_skeleton_positions: Array = []
var _mesh_to_local_bone: Array        = []
var _global_bone_to_global: Transform3D

var _initial_transform: Transform3D

# Called when the node enters the scene tree for the first time.
func _ready():
	_initial_transform = self.transform
	_compute_forward_vector()
	_compute_bone_transforms()
	
	var mesh: MeshInstance3D = get_node( "Armature/Skeleton3D/Circle" )
	_material = mesh.get_surface_override_material( 0 )





func apply( broad_tree: BroadTreeGd, meshes: Array, vel_in_mesh: Vector3, se3_mesh_to_rf: Se3Ref, se3_rf_to_mesh: Se3Ref ):
	# Orient along velocity.
	if random_angle:
		angle = randf() * 6.29
		#angle = 0.0
	
	var adjustment_t: Transform3D = _compute_orientation( vel_in_mesh, angle )
	#adjustment_t                = Transform.IDENTITY
	var own_t: Transform3D = _initial_transform
	own_t = adjustment_t * own_t
	self.transform = own_t
	
	#var global_t: Transform = self.global_transform
	
	var finish_in_local: Vector3 = own_t.origin
	var t_mesh_to_rf: Transform3D = se3_mesh_to_rf.transform
	var finish_in_rf: Vector3    = t_mesh_to_rf * (finish_in_local)
	
	var t_rf_to_mesh: Transform3D = se3_rf_to_mesh.transform
	
	var global_bone_to_global: Transform3D = own_t * _global_bone_to_global
	var global_to_global_bone: Transform3D = global_bone_to_global.inverse()
	
	var s: Skeleton3D = _get_skeleton()
	var qty: int = _bone_skeleton_positions.size()
	
	# This one is for debugging.
	var global_poses: Array = []
	
	for i in range(qty):
		var bone_at: Vector3 = _bone_skeleton_positions[i]
		var bone_at_in_mesh: Vector3 = global_bone_to_global * (PROBE_DIST * bone_at)
		var bone_at_in_rf: Vector3 = t_mesh_to_rf * (bone_at_in_mesh)
		var start_in_rf: Vector3 = finish_in_rf + bone_at_in_rf
		var ret: Array = broad_tree.intersects_segment_face( start_in_rf, finish_in_rf, null )
		var intersects: bool = ret[0]
		
		if intersects:
			var mesh_gd: OctreeMeshGd = ret[6]
			var in_the_list: bool = meshes.has( mesh_gd )
			intersects = intersects and in_the_list
		
		var bone_t: Transform3D = s.get_bone_pose( i )
		if not intersects:
			bone_t.origin = bone_at
		
		else:
			var intersection_in_rf: Vector3 = ret[2]
			var dr: Vector3 = intersection_in_rf - finish_in_rf
			var at_in_rf: Vector3 = finish_in_rf + dr * 1.0
			var to_local_t: Transform3D = _mesh_to_local_bone[i]
			to_local_t = to_local_t * global_to_global_bone * t_rf_to_mesh
			var in_bone_at: Vector3 = to_local_t * (at_in_rf)
			bone_t.origin = in_bone_at
		
		#s.set_bone_pose( i, bone_t )
		s.set_bone_pose_position( i, bone_t.origin )
		
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
	var qty: int    = s.get_bone_count()

	var skeleton_t: Transform3D = s.transform
	var armature_t: Transform3D = get_node( "Armature" ).transform
	var total_t: Transform3D    = armature_t * skeleton_t

	_global_bone_to_global   = total_t
	_bone_skeleton_positions = []
	_mesh_to_local_bone      = []
	for i in range(qty):
		var bone_t: Transform3D = s.get_bone_global_pose( i )
		_bone_skeleton_positions.push_back( bone_t.origin )
		
		var rest_t: Transform3D         = s.get_bone_rest( i )
		var custom_t: Transform3D       = Transform3D.IDENTITY #s.get_bone_custom_pose( i )
		var bone_to_global: Transform3D = total_t * rest_t * custom_t
		var global_to_bone: Transform3D = bone_to_global.inverse()
		_mesh_to_local_bone.push_back( global_to_bone )


func _compute_orientation(v: Vector3, roll_angle: float = 0.0):
	var t: Transform3D = _initial_transform
	var forward: Vector3 = t.basis * (_forward_vector)
	# compute rotation converting "forward" to "v".
	var v_norm: Vector3 = v.normalized()
	var a: Vector3 = forward.cross( v_norm )
	var si: float = a.length()
	var q: Quaternion
	
	if si < EPS:
		q = Quaternion.IDENTITY
	
	else:
		var co: float      = forward.dot( v_norm )
		var angle_2: float = atan2( si, co ) * 0.5
		var si2: float = sin(angle_2)
		var co2: float = cos(angle_2)
		a   = a.normalized()
		q.x = a.x*si2
		q.y = a.y*si2
		q.z = a.z*si2
		q.w = co2
	
	# Additional rotation around forward vector.
	var roll_q: Quaternion = Quaternion( _forward_vector, roll_angle )
	q = q * roll_q
	
	var b: Basis = Basis(q)
	var tr: Transform3D = Transform3D( b, Vector3.ZERO )
	
	return tr


func setup_material( speed: float, color_speed: float ):
	# This one defines how quickly noise texture is moving.
	_material.set_shader_parameter( "speed",       speed )
	# This one defines color. Its valid range is [0, 1].
	_material.set_shader_parameter( "color_speed", color_speed )




