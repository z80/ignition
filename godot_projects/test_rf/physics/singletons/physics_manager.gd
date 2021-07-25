
extends Node


# All visualization meshes/or other types are with respect to the ref frame 
# where controlled object is.
# Ref frame is selected when player clicks the icon.
var player_select = null setget _set_player_select, _get_player_select
# Ref frame gets focus when explicitly pressed "c" (center) on a selected ref. frame.
var player_control = null setget _set_player_control, _get_player_control
# Camera. It is supposed to be the only one.
var camera = null setget _set_camera, _get_camera
# Scaler for large distances.
var distance_scaler = DistanceScalerRef.new()


# Called when the node enters the scene tree for the first time.
func _ready():
	init()


func init():
	distance_scaler.plain_distance = 100.0
	distance_scaler.log_scale      = 1.0


# Update body visual parts in accordance with what is set to be 
# player ref. frame. (The ref. frame where the camera is located.)
func _process(_delta):
	
	var player_rf = get_player_ref_frame()
	if player_rf == null:
		return
	update_bodies_visual()
	update_providers()
	update_planets( _delta )
	update_camera()
	
	# Relocate children of celestial bodies depending on the 
	# gravitational influence and atmosphere bounds.
	process_celestial_body_children()




func _physics_process( delta ):
	# Need to have all super body poses up to date.
	update_super_bodies()
	
	var group: String = Constants.REF_FRAME_PHYSICS_GROUP_NAME
	var ref_frames: Array = get_tree().get_nodes_in_group( group )
	
	for rf in ref_frames:
		var deleted: bool =  rf.is_queued_for_deletion()
		if deleted:
			continue
		rf.evolve( delta )
	
	for rf in ref_frames:
		var deleted: bool =  rf.is_queued_for_deletion()
		if deleted:
			continue
		rf.process_children()
	
	# Update. Here controls are applied.
	update_bodies_physical( delta )




func _input( event ):
	return
	
	#var body: Body = player_focus as Body
	#if body == null:
	#	return
	
	# Call user input for the upper most body.
	# It is supposed to distribute the same controls 
	# to all sub-bodies.
	#var sb = body.super_body
	#while sb != null:
	#	body = sb
	#	sb = body.super_body
	#body.process_user_input( event )


func physics_ref_frames():
	var group: String = Constants.REF_FRAME_PHYSICS_GROUP_NAME
	var ref_frames: Array = get_tree().get_nodes_in_group( group )
	return ref_frames


# Called externally by UserInput global object.
func process_user_input( input: Dictionary ):
	#print( "user_input: ", input )
	
	var body: Body = player_control as Body
	if body == null:
		return
	
	# Call user input for the upper most body.
	# It is supposed to distribute the same controls 
	# to all sub-bodies.
	var sb: RefFrameNode = body.root_most_body()
	sb.process_user_input_2( input )









func update_super_bodies():
	var group: String = Constants.SUPER_BODIES_GROUP_NAME
	var all_bodies: Array = get_tree().get_nodes_in_group( group )
	for body in all_bodies:
			body.update_pose()


func update_bodies_visual():
	var player_rf = get_player_ref_frame()
	
	# Update visuals for all the physical-visual objects.
	var group: String = Constants.BODIES_GROUP_NAME
	var all_bodies: Array = get_tree().get_nodes_in_group( group )
	for body in all_bodies:
		body.update_visual( player_rf )



func update_bodies_physical( delta: float ):
	var player_rf = get_player_ref_frame()
	if player_rf == null:
		return
	
	# Update visuals for all the physical-visual objects.
	var group: String = Constants.BODIES_GROUP_NAME
	var bodies = get_tree().get_nodes_in_group( group )
	for body in bodies:
		body.update_physical( delta )
	
	# And update super body poses.
	group = Constants.SUPER_BODIES_GROUP_NAME
	bodies = get_tree().get_nodes_in_group( group )
	for body in bodies:
		body.update_pose()



func update_providers():
	var group: String = Constants.PROVIDERS_GROUP_NAME
	var surface_providers: Array = get_tree().get_nodes_in_group( group )
	for pr in surface_providers:
		pr.update()


func update_planets( delta: float ):
	var group: String = Constants.PLANETS_GROUP_NAME
	var all_spheres: Array = get_tree().get_nodes_in_group( group )
	for sphere in all_spheres:
		sphere.process( delta )


func force_rebuild_visual_spheres():
	var group: String = Constants.PLANETS_GROUP_NAME
	var all_spheres: Array = get_tree().get_nodes_in_group( group )
	for sphere in all_spheres:
		sphere.force_rebuild()
	

func update_sun():
	var group: String = Constants.SUN_GROUP_NAME
	var all_suns: Array = get_tree().get_nodes_in_group( group )
	for sun in all_suns:
		sun.process()


func process_celestial_body_children():
	var group: String = Constants.CELESTIAL_BODIES_NAME
	var celestial_bodies: Array = get_tree().get_nodes_in_group( group )
	for cb in celestial_bodies:
		cb.process_ref_frames( celestial_bodies )


func update_camera():
	var ClosestForceSource = load( "res://physics/utils/closest_force_source.gd" )
	# Update camera orientation.
	var pc: Body = PhysicsManager.player_control
	if pc == null:
		return
	
	var rf: RefFrameNode = ClosestForceSource.closest_force_source( pc )
	if rf == null:
		return
	
	if rf.force_source == null:
		return
	
	var defines_vertical: bool = rf.force_source.defines_vertical()
	var c: Camera = PhysicsManager.camera
	if defines_vertical:
		if pc == null:
			return
		if not c.has_method( "process_basis" ):
			return
		var up: Vector3 = rf.force_source.up( rf, pc )
		up = pc.q().xform( up )
		c.process_basis( up )
		
	# For the body under player control find the closest celestial
	# body. If found, specify the atmosphere parameters.
	var ClosestCelestialBody = load( "res://physics/utils/closest_celestial_body.gd" )
	var p_rf = get_player_ref_frame()
	if p_rf == null:
		return
	var celestial_body: Node = ClosestCelestialBody.closest_celestial_body( p_rf )
	if celestial_body != null:
		c.apply_atmosphere( p_rf, celestial_body )
	
	# Apply sun.
	var group: String = Constants.SUN_GROUP_NAME
	var all_suns: Array = get_tree().get_nodes_in_group( group )
	if not all_suns.empty():
		var sun: RefFrameNode = all_suns[0] as RefFrameNode
		c.apply_sun( p_rf, sun )



func create_ref_frame_physics():
	var rf: RefFramePhysics = RefFramePhysics.new()
	return rf


#func create_ref_frame_physics():
#	var available: int = available_qty()
#	if available < 1:
#		return null
#	var rf: RefFramePhysics = RefFramePhysics.new()
#	rf.init_physics()
#	return rf



func _set_player_ref_frame( _rf ):
	#player_ref_frame = rf
	pass


func _get_player_ref_frame():
	#return player_ref_frame
	var body: Body = self.player_control
	var p: Node = body.get_parent()
	var rf_p: RefFramePhysics = p as RefFramePhysics
	
	return rf_p


func get_player_ref_frame():
	var body: Body = self.player_control
	if body == null:
		return null
	var p: Node = body.get_parent()
	var rf_p: RefFramePhysics = p as RefFramePhysics
	
	return rf_p


func _set_player_select( rf ):
	if is_instance_valid( player_select ):
		var b = player_select as Body
		if b:
			b.process_user_input_2( {} )
	player_select = rf


func _get_player_select():
	return player_select


func _set_player_control( rf ):
	if is_instance_valid( player_control ):
		var b = player_control as Body
		if b:
			b.process_user_input_2( {} )
	player_control = rf


func _get_player_control():
	return player_control


func _set_camera( cam ):
	camera = cam


func _get_camera():
	return camera



func serialize():
	var data: Dictionary = {}
	if player_select != null:
		data["select"] = player_select.get_path()
	else:
		data["select"] = ""

	if player_control != null:
		data["control"] = player_control.get_path()
	else:
		data["control"] = ""
	return data



func deserialize( data: Dictionary ):
	var select_path: String = data["select"]
	player_select = get_node( select_path )
	var control_path: String = data["control"]
	player_control = get_node( control_path )
	return true


