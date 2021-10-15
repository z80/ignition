
extends Spatial
class_name EntranceNode

# Boarding works within this distance 
# from the node's origin.
# Unboarding happens in the node origin. So it shouldn't be inside of 
# the habitat module.
export(float) var radius = 10.0

var relative_to_owner: Transform = Transform.IDENTITY
# Part pointer. This thing should be filled externally.
var part: Node = null


# Called when the node enters the scene tree for the first time.
func _ready():
	relative_to_owner = compute_relative_to_owner()


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





# Check if player controlled entity is within the radius.
func characters_for_boarding():
	if part == null:
		return null
	
	var ctrl: Node = PhysicsManager.player_control
	if (ctrl == null) or (not is_instance_valid(ctrl)):
		return null
	
	# Only characters can board. 
	var is_character: bool = ctrl.has_method( "is_character" ) and ctrl.is_character()
	if not is_character:
		return null
	
	var se3: Se3Ref = Se3Ref.new()
	se3.transform = relative_to_owner
	se3 = ctrl.relative_to_se3( part, se3 )
	var r: Vector3 = se3.r
	var d: float = r.length()
	if d > radius:
		return null
	
	return ctrl





