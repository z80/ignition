
extends RefFrameNode
class_name PartAssembly

# This class handles dynamic body group.
# On every update it should average it's position to the mean of 
# all sub-bodies.


# List of sub-bodies this body contains if it is a super-body.
var sub_bodies: Array = []


func _ready():
	add_to_group( Constants.SUPER_BODIES_GROUP_NAME )


func change_parent( new_parent: Node = null ):
	.change_parent( new_parent )
	
	# First go down to the lowest level.
	for body in sub_bodies:
		body.change_parent( new_parent )



func add_sub_body( body: RefFrameNode ):
	var sb: RefFrameNode = body.super_body
	if (sb != null) and (sb != self):
		sb.remove_sub_body( body )
	
	var has: bool = has_sub_body( body )
	if has:
		return false
	
	sub_bodies.push_back( body )
	body.super_body = self
	return true



func remove_sub_body( body: RefFrameNode ):
	var index: int = sub_bodies.find( body )
	if index >= 0:
		sub_bodies.remove( index )
		body.super_body = null


func is_super_body():
	var empty: bool = sub_bodies.empty()
	var ret: bool = not empty
	return ret


func has_sub_body( body: RefFrameNode, recursive: bool = true ):
	var has: bool = sub_bodies.has( body )
	if has:
		return true
	
	if recursive:
		for sb in sub_bodies:
			has = sb.has_sub_body( body, true )
			if has:
				return true
	
	return false


func distance_max( other: RefFrameNode ):
	if other.sub_bodies.empty():
		var d: float = distance( other )
		return d
	
	var max_d: float = -1.0
	for cb in other.sub_bodies:
		var d: float = distance_max( cb )
		if (d > max_d) or (max_d < 0.0):
			max_d = d
	
	return max_d



func distance_min( other: RefFrameNode ):
	if other.sub_bodies.empty():
		var d: float = distance( other )
		return d
	
	var min_d: float = -1.0
	for cb in other.sub_bodies:
		var d: float = distance_min( cb )
		if (d < min_d) or (min_d < 0.0):
			min_d = d
	
	return min_d



func distance( other: RefFrameNode ):
	var se3: Se3Ref = self.relative_to( other )
	var r: Vector3 = se3.r
	var d: float = r.length()
	return d






func process_inner( delta ):
	# Loop over all sub bodies.
	var at: Vector3 = Vector3.ZERO
	for body in sub_bodies:
		var r: Vector3 = body.r()
		at += r
	
	var qty: int = sub_bodies.size()
	if qty > 0:
		at = at / qty
	
	self.set_r( at )




func process_user_input_2( input: Dictionary ):
	for body in sub_bodies:
		body.process_user_input_2( input )







func activate( root_call: bool = true ):
	for body in sub_bodies:
		body.activate()



func deactivate( root_call: bool = true ):
	for body in sub_bodies:
		body.deactivate()




func save():
	return {}



func load( data: Dictionary ):
	return true



