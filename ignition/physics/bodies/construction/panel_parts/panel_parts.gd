extends Control


var construction = null

signal create_block

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Frames_pressed():
	var frames = find_child( "ContainerFrames" )
	var parts = find_child( "ContainerParts" )
	frames.visible = true
	parts.visible  = false
	
	_create_buttons_frames()


func _on_Parts_pressed():
	var frames = find_child( "ContainerFrames" )
	var parts = find_child( "ContainerParts" )
	frames.visible = false
	parts.visible  = true
	
	_create_buttons_parts()


func _cleanup_panel( panel ):
	var children = panel.get_children()
	for ch in children:
		ch.queue_free()


func _add_button( panel, res_icon: String, block_name: String, desc: String, dynamic: bool=true ):
	var Btn = load( "res://physics/bodies/construction/panel_parts/item_button.tscn" )
	var icon = load( res_icon )
	var btn = Btn.instantiate()
	panel.add_child( btn )
	btn.icon       = icon
	btn.block_name = block_name
	btn.description = desc
	btn.dynamic     = dynamic
	# Connecting signal directly to construction.
	btn.connect("create_block", Callable(construction, "create_block"))


func _create_buttons_frames():
	var frames = find_child( "ContainerFrames" )
	if frames == null:
		return
	_cleanup_panel( frames )
	
	_add_button( frames, "res://physics/parts/frames/frame_box/icon.png", 
						"frame_box", 
						"Frame box description", false )

	_add_button( frames, "res://physics/parts/frames/frame_light/assets/icon.png", 
						"frame_light", 
						"Frame light description", false )


func _create_buttons_parts():
	var parts = find_child( "ContainerParts" )
	if parts == null:
		return
	_cleanup_panel( parts )
	
	
	_add_button( parts, "res://physics/parts/fuel_tanks/2m/3m_fuel/assets/icon.png", 
						"fuel_tank_2m_3m", 
						"Fuel tank 2m diameter, 3m height", true )
	
	_add_button( parts, "res://physics/parts/fuel_tanks/2m/3m_oxidizer/assets/icon.png", 
						"oxidizer_tank_2m_3m", 
						"Oxidizer tank 2m diameter, 3m height", true )
	
	_add_button( parts, "res://physics/parts/fuel_tanks/2m/5m_fuel/assets/icon.png", 
						"fuel_tank_2m_5m", 
						"Fuel tank 2m diameter, 5m height", true )
	
	_add_button( parts, "res://physics/parts/fuel_tanks/2m/5m_oxidizer/assets/icon.png", 
						"oxidizer_tank_2m_5m", 
						"Oxidizer tank 2m diameter, 5m height", true )


#	_add_button( parts, "res://physics/parts/fuel_tanks/2m/3m_fuel/assets/icon.png", 
#						 "fuel_tank_2m_3m", 
#						 "OSP Fuel tank 2m diameter, 3m height", true )
#
#	_add_button( parts, "res://physics/parts/fuel_tanks/2m/3m_oxidizer/assets/icon.png", 
#						 "oxidizer_tank_2m_3m", 
#						 "OSP oxidizer tank 2m diameter, 3m height", true )
	
	
#	_add_button( parts, "res://physics/parts/2m/reaction_wheels/assets/icon.png", 
#						 "reaction_wheels_2m", 
#						 "2m reaction control wheels", true )

#	_add_button( parts, "res://physics/parts/thrusters/thruster_01/assets/icon.png", 
#						 "thruster_01", 
#						 "Thrsuter ver 01", true )

	_add_button( parts, "res://physics/parts/thrusters/atmospheric/assets/icon.png", 
						"thruster_atmospheric", 
						"Low altitude atmosphere thrsuter", true )

	_add_button( parts, "res://physics/parts/thrusters/vacuum/assets/icon.png", 
						"thruster_vacuum", 
						"High altitude vacuum optimized thrsuter", true )
	
	
	_add_button( parts, "res://physics/parts/reaction_control_wheels/2m/m1/assets/icon.png", 
						"reaction_control_wheels_2m_m1", 
						"2m reaction control wheels", true )
	
	_add_button( parts, "res://physics/parts/habitats/2m/m1/assets/icon.png", 
						"habitat_2m_m1", 
						"Habitat 2m diameter", true )
	
	_add_button( parts, "res://physics/parts/decouplers/2m/m1/assets/icon.png", 
						"decoupler_2m_1m", 
						"Decoupler 2m diameter", true )
	












