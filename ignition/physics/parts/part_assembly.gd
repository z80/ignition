
extends RefFrameAssemblyNode
class_name PartAssembly

# This class handles dynamic body group.
# On every update it should average it's position to the mean of 
# all sub-bodies.



# For visualizing the orbit it is needed to have an OrbitalMotionRef in order 
# to have points on the trajectory.
var motion: CelestialMotionRef = null
var orbit_visualizer: Node = null
var closest_celestial_body: RefFrameNode = null
@export var orbit_color: Color = Color( 0.0, 0.7, 0.0, 1.0 )
@export var show_orbit: bool = false: get = _get_show_orbit, set = _set_show_orbit


#func get_class():
#	return "PartAssembly"



#func queue_free():
#	super.queue_free()



func _exit_tree():
	if is_queued_for_deletion():
		on_delete()



func _enter_tree():
	# The thing is it is necessary to be sure that rigid bodies 
	# on both sides are attached. And due to that it is necessary 
	# to first create all bodies in a loop and after that activate 
	# joints.
	# After physics bodies are created for all sub-bodies, 
	# Create connecting joints.
	var qty: int = get_child_count()
	for i in range(qty):
		var b: Node = get_child( i )
		# Only parts have this method, so check if it exists first.
		var has: bool = b.has_method( "activate_nodes" )
		if has:
			b.activate_nodes( false )
	pass



func _ready():
	add_to_group( Constants.BODY_ASSEMBLIES_GROUP_NAME )
	_create_orbit_visualizer()




func _process( _delta: float ):
	_process_visualize_orbits()
	pass






func has_player_control():
	var pc = RootScene.ref_frame_root.player_camera.get_parent()
	var p: Node = pc.get_parent();
	var ret: bool = (p == self)
	return ret



func _change_parent( p: Node, recursive_call: bool ):
	# The thing is it is necessary to be sure that rigid bodies 
	# on both sides are attached. And due to that it is necessary 
	# to first create all bodies in a loop and after that activate 
	# joints.
	# After physics bodies are created for all sub-bodies, 
	# Create connecting joints.
	var qty: int = get_child_count()
	for i in range(qty):
		var b: Node = get_child( i )
		# Only parts have this method, so check if it exists first.
		var has: bool = b.has_method( "activate_nodes" )
		if has:
			b.activate_nodes( false )


func distance_max( other: RefFrameNode ):
	if other.sub_bodies.is_empty():
		var d: float = distance( other )
		return d
	
	var max_d: float = -1.0
	for cb in other.sub_bodies:
		var d: float = distance_max( cb )
		if (d > max_d) or (max_d < 0.0):
			max_d = d
	
	return max_d



func distance_min( other: RefFrameNode ):
	if other.sub_bodies.is_empty():
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











func process_user_input_2( input: Dictionary ):
	var qty: int = get_child_count()
	for i in range(qty):
		var ch: Node = get_child( i )
		var body: RefFrameBodyNode = ch as RefFrameBodyNode
		if body != null:
			body.process_user_input_2( input )



func show_click_container():
	return true


func gui_mode():
	return []


func activate( root_call: bool = true ):
	var qty: int = get_child_count()
	for i in range(qty):
		var ch: Node = get_child(i)
		var body: RefFrameBodyNode = ch as RefFrameBodyNode
		if body != null:
			body.activate()



func deactivate( root_call: bool = true ):
	var qty: int = get_child_count()
	for i in range(qty):
		var ch: Node = get_child(i)
		var body: RefFrameBodyNode = ch as RefFrameBodyNode
		if body != null:
			body.deactivate()



func gui_classes( mode: Array ):
	return []



func _create_orbit_visualizer():
	motion = CelestialMotionRef.new()
	
	var Vis = load( "res://physics/celestial_bodies/orbit_visualizer.tscn" )
	orbit_visualizer = Vis.instantiate()
	
	var layer: Node3D = RootScene.get_visual_layer_space()
	layer.add_child( orbit_visualizer )
	
	orbit_visualizer.color     = orbit_color
	_set_show_orbit( false )



func _set_orbit_color( c: Color ):
	orbit_color = c
	if orbit_visualizer != null:
		orbit_visualizer.color = orbit_color

func _get_orbit_color():
	return orbit_color

func _set_show_orbit( en: bool ):
	show_orbit = en
	orbit_visualizer.visible = show_orbit
	orbit_visualizer.color   = orbit_color


func _get_show_orbit():
	return show_orbit


func _process_visualize_orbits():
	var root: RefFrameRoot = get_ref_frame_root()
	var new_state: bool = root.visualize_orbits
	var current_state: bool = self.show_orbit
	if current_state != new_state:
		self.show_orbit = new_state
	if show_orbit or true:
		# Here additionally we need to initialize celestial motion every single time.
		_update_celestial_motion()
		orbit_visualizer.draw( closest_celestial_body, motion )


func _update_celestial_motion():
	var ClosestCelestialBody = preload( "res://physics/utils/closest_celestial_body.gd" )
	var p: Node = self.get_parent()
	var cb: CelestialBody = ClosestCelestialBody.closest_celestial_body( p )
	if cb == null:
		#_do_show = false
		closest_celestial_body = null
		return
	closest_celestial_body = cb
	var se3: Se3Ref = self.relative_to( cb )
	motion.allow_orbiting = true
	motion.launch( cb.own_gm, se3 )
	
	# Check out what's the mode.
	var m: String = motion.movement_type()
	#print( "movement_type: ", m )



func on_delete():
	self.name = self.name + "_to_be_deleted"
	#var bodies: Array = sub_bodies.duplicate()
	#for b in bodies:
	#	if is_instance_valid( b ):
	#		remove_sub_body( b )



func get_ref_frame_root():
	var rf: RefFrameNode = RootScene.ref_frame_root
	return rf

