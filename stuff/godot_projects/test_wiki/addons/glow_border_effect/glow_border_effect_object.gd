class_name GlowBorderEffectObject, "res://addons/glow_border_effect/glow_border_effect_renderer_icon.png"
extends Spatial
# Class to apply to object that shall glow.
# Apply the GlowBorderEffectObject to a spatial node that hold
# GeometryInstances and that should have the glowing border effect
# applied either statically through editor or dynamically
# by calling the set_glow_border_effect function

# Configuration of the glow color
export var glow_color : Color = Color.yellow

# Configuration of the visual layer to use for drawing of shadow meshes
export(int, LAYERS_3D_RENDER) var effect_layer = 0x400

# Enable or disable the glow effect, either through
# editor value or through the set_glow_border_effect function
export var glow_border_effect : bool = false setget set_glow_border_effect

# Hold reference to created shadow objects used for glow rendering
var _glow_shadow_objects : Array


# Called when the node enters the scene tree for the first time.
func _ready():
	var glow_material = SpatialMaterial.new()
	glow_material.albedo_color = glow_color
	
	# Create shadow meshes for all GeometryInstances
	# for glow effect rendering
	_create_shadow_meshes(self, glow_material)


func set_glow_border_effect(val : bool):
	glow_border_effect = val
	for glow_object in _glow_shadow_objects:
		glow_object.set_visible(val)


# Create shadow meshes for all GeometryInstances
# for glow effect rendering
func _create_shadow_meshes(obj, glow_material):
	# Recurse down the stucture in case
	# GeometryInstance exists as childs
	for child in obj.get_children():
		_create_shadow_meshes(child, glow_material)
	
	# Create shadow meshes for GeometryInstances
	if obj is GeometryInstance:
		var glow_object = obj.duplicate()
		glow_object.layers = effect_layer
		glow_object.set_material_override(glow_material)
		
		# Clean up and remove StaticBody
		for sub in glow_object.get_children():
			if sub is StaticBody:
				glow_object.remove_child(sub)
				
		# Remove scripts
		glow_object.set_script(null)
		
		# Ensure objects glow according setting
		glow_object.set_visible(glow_border_effect)
		
		# Now add the new shadow object to the tree
		obj.add_child(glow_object)
		_glow_shadow_objects.append(glow_object)
