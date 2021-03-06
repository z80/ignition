
extends RefFrameNode
class_name PartAssembly

# This class handles dynamic body group.
# On every update it should average it's position to the mean of 
# all sub-bodies.


# List of sub-bodies this body contains if it is a super-body.
var sub_bodies: Array = []


func get_class():
	return "PartAssembly"


func _enter_tree():
	# If physics body is already created it shouldn't hurt 
	# anything.
	for b in sub_bodies:
		b.create_physical()
	# The thing is it is necessary to be sure that rigid bodies 
	# on both sides are attached. And due to that it is necessary 
	# to first create all bodies in a loop and after that activate 
	# joints.
	# After physics bodies are created for all sub-bodies, 
	# Create connecting joints.
	for b in sub_bodies:
		# Only parts have this method, so check if it exists first.
		var has: bool = b.has_method( "activate_nodes" )
		if has:
			b.activate_nodes( false )



func _ready():
	add_to_group( Constants.SUPER_BODIES_GROUP_NAME )





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


func has_sub_body( body: RefFrameNode ):
	var has: bool = sub_bodies.has( body )
	if has:
		return true
	
	return false


func has_player_control():
	var pc = PhysicsManager.player_control
	for body in sub_bodies:
		if body == pc:
			return true
	
	return false



func change_parent( p: Node = null ):
	#var t_before: Transform = self.transform
	#var se3: Se3Ref = self.relative_to( p )
	.change_parent( p )
	
	# First need to remove all physical joints.
	# Physical joints are being removed within 
	# remove_physical() call.
	
	#var t_after: Transform = self.transform
	for b in sub_bodies:
		#t_before = b.transform
		b.change_parent_inner( p )
		#t_after = b.transform
	
	# The thing is it is necessary to be sure that rigid bodies 
	# on both sides are attached. And due to that it is necessary 
	# to first create all bodies in a loop and after that activate 
	# joints.
	# After physics bodies are created for all sub-bodies, 
	# Create connecting joints.
	for b in sub_bodies:
		# Only parts have this method, so check if it exists first.
		var has: bool = b.has_method( "deactivate_nodes" )
		if has:
			b.activate_nodes( false )


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






func update_pose():
	# Loop over all sub bodies.
	var r_accum: Vector3 = Vector3.ZERO
	var v_accum: Vector3 = Vector3.ZERO
	for body in sub_bodies:
		var r: Vector3 = body.r()
		r_accum += r
		var v: Vector3 = body.v()
		v_accum += v
	
	var qty: int = sub_bodies.size()
	if qty > 0:
		r_accum = r_accum / qty
		v_accum = v_accum / qty
	
	self.set_r( r_accum )
	self.set_v( v_accum )




func process_user_input_2( input: Dictionary ):
	for body in sub_bodies:
		body.process_user_input_2( input )



func root_most_body():
	return self


func show_click_container():
	return true


func gui_mode():
	return []


func activate( root_call: bool = true ):
	for body in sub_bodies:
		body.activate()



func deactivate( root_call: bool = true ):
	for body in sub_bodies:
		body.deactivate()



func gui_classes( mode: Array ):
	return []



func save():
	return {}



func load( data: Dictionary ):
	return true



