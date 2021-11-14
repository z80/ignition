
extends Spatial
class_name CouplingNode

# The part can be connected to others via this node.
export(bool) var allows_connecting = true
# Other parts can be connected to this node.
export(bool) var allows_connections = true
# Allows getting connected to the surface
export(bool) var allows_surface_coupling = true

enum NodeSize { SMALL=0, MEDIUM=1, LARGE=2 }
export(NodeSize) var node_size = NodeSize.MEDIUM

# Reference to the part.
# Can't use "owner" field as it shows the Visual's root which 
# isn't the part itself.
var part: RefFrameNode = null

# Reference to connection created (or not created).
var connection: Reference = null

var relative_to_owner: Transform = Transform.IDENTITY

# Angle only transform.
#var angle_transform: Transform = Transform.IDENTITY

# Fordisplaying visuals.
export(bool) var show_visual = false setget _set_show_visual, _get_show_visual
var _visual = null





# Called when the node enters the scene tree for the first time.
func _ready():
	relative_to_owner = compute_relative_to_owner()


# This thing should be called when in kinematic mode.
# Is should position child parts relatie to parent parts in the right way.
# Should be overwritten in derived classes.
func process():
	if show_visual and (_visual != null):
		var t: Transform = world_transform()
		_visual.transform = t
		_visual.size = snap_size()


func connected():
	var ret: bool = (connection != null)
	return ret


func compute_relative_to_owner():
	var p: Node = self
	var t: Transform = Transform.IDENTITY
	var ret: Transform = _compute_relative_to_owner_recursive( p, t )
	return ret



func _compute_relative_to_owner_recursive( n: Node, t: Transform ):
	var s: Spatial = n as Spatial
	if s != null:
		var ct: Transform = s.transform
		t = t * ct
	var ow: Node = self.owner
	if n == ow:
		return t
	
	var p: Node = get_parent()
	var ret: Transform = _compute_relative_to_owner_recursive( p, t )
	return ret





func _set_show_visual( en: bool ):
	show_visual = en
	if show_visual:
		# Don't show ones which are already connected.
		var c: bool = connected()
		if c:
			return
		
		if _visual == null:
			var Visual = preload( "res://physics/parts/coupling_nodes/coupling_node_visual.tscn" )
			_visual = Visual.instance()
			_visual.size = snap_size()
			var vp: Viewport = RootScene.get_overlay_viewport()
			vp.add_child( _visual )
		_visual.visible = true
	else:
		if _visual != null:
			_visual.queue_free()
			_visual = null
	
	# Make overlay 3d viewport visible.
	RootScene.set_overlay_visible( en )



func _get_show_visual():
	return show_visual




func world_transform():
	var camera: RefFrameNode = PhysicsManager.camera
	var se3: Se3Ref = part.relative_to( camera )
	var t_parent: Transform = se3.transform
	var t: Transform = t_parent * relative_to_owner
	return t


func snap_size():
	if node_size == NodeSize.SMALL:
		return Constants.NODE_SIZE_SMALL
	elif node_size == NodeSize.MEDIUM:
		return Constants.NODE_SIZE_MEDIUM
	
	return Constants.NODE_SIZE_LARGE







func couple_with( n: CouplingNode ):
	if not allows_connecting:
		return false
	
	var node_stacking: CouplingNode = n as CouplingNode
	if node_stacking == null:
		return false
	
	if not node_stacking.allows_connections:
		return false
	
	# Now measure the distance.
	var t_w: Transform = world_transform()
	var n_t_w: Transform = n.world_transform()
	var r_w: Vector3 = t_w.origin
	var n_r_w: Vector3 = n_t_w.origin
	var d: float = (r_w - n_r_w).length()
	
	var max_d: float = snap_size() + n.snap_size()
	if max_d < d:
		return false
	
	# No more excuses, couple these two together.
	# TODO: need to implement it for newly introduced
	# connections.
	assert( false )
#	node_b_path = n.get_path()
#	node_b      = n
#	is_parent   = false
#
#	node_b.node_b_path = get_path()
#	node_b.node_b      = self
#	node_b.is_parent   = true
	
	return true


func decouple():
	# Need to implement all these using newly introduced 
	# connections.
	assert( false )
#	if node_b_path == null:
#		return
#	node_b = get_node( node_b_path )
#	if node_b == null:
#		node_b_path = null
#		return
#
#	node_b.node_b_path = null
#	node_b.node_b      = null
#	node_b.is_parent   = false
#
#	node_b_path = null
#	node_b      = null
#	is_parent   = false
	


