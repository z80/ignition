extends ViewportContainer
# Collection of viewports and shaders to create the glowing border effect
# The GlowBorderEffectRender configure the needed viewports and
# ViewportContainers to create the glowing border effect.
# To align the internal cameras with the current camera of your
# scene call the camera_transform_changed.

# Cull mask for cameras
## Set the cull mask used to view the visuall layer defined
## for the GlowBorderEffectObject
export(int, LAYERS_3D_RENDER) var effect_cull_mask = 0x00400 setget set_effect_cull_mask
## Set the cull mask use to render the scene. Should
## not include the effect_cull_mask bit.
export(int, LAYERS_3D_RENDER) var scene_cull_mask = 0xffbff setget set_scene_cull_mask
## Set the intensity of the border
export(float, 0.0, 5.0, 0.1) var intensity = 3.0 setget set_intensity

# Create references to cameras
onready var camera_prepass = $ViewportBlure/ViewportContainerBlureX/ViewportHalfBlure/ViewportContainerBlureY/ViewportPrepass/Camera
onready var camera_scene = $ViewportScene/Camera

# Create references to viewports
onready var view_prepass = $ViewportBlure/ViewportContainerBlureX/ViewportHalfBlure/ViewportContainerBlureY/ViewportPrepass
onready var view_half_blure = $ViewportBlure/ViewportContainerBlureX/ViewportHalfBlure
onready var view_blure = $ViewportBlure
onready var view_scene = $ViewportScene

# Create references to viewport containers
onready var container_gaussian_y = $ViewportBlure/ViewportContainerBlureX/ViewportHalfBlure/ViewportContainerBlureY
onready var container_gaussian_x = $ViewportBlure/ViewportContainerBlureX


# Called when the node enters the scene tree for the first time.
func _ready():
	# Setup shader inputs
	material.set_shader_param("intensity", intensity)
	material.set_shader_param("view_prepass", view_prepass.get_texture())
	material.set_shader_param("view_blure", view_blure.get_texture())
	material.set_shader_param("view_scene", view_scene.get_texture())
	
	# Ensure that the internal cameras cull sceen and shadow objects
	camera_prepass.cull_mask = effect_cull_mask
	camera_scene.cull_mask = scene_cull_mask
	
	# Resize all internal views
	resize()


# Setter function for the effect_cull_mask. Ensure update of prepass camera
func set_effect_cull_mask(val):
	effect_cull_mask = val
	if camera_prepass:
		camera_prepass.cull_mask = effect_cull_mask


# Setter function for the effect_cull_mask. Ensure update of scene camera
func set_scene_cull_mask(val):
	scene_cull_mask = val
	if camera_scene:
		camera_scene.cull_mask = scene_cull_mask


# Setter function for the intensity. Enusre update of the internal shader
func set_intensity(val):
	intensity = val
	material.set_shader_param("intensity", intensity)


# Call this to resize all the internal views of the GlowBorderEffectRenderer
func resize():
	view_prepass.size = rect_size
	view_half_blure.size = rect_size
	view_blure.size = rect_size
	view_scene.size = rect_size
	container_gaussian_x.rect_size = rect_size
	container_gaussian_y.rect_size = rect_size


# Call this function to align the internal cameras in the
# GlowBorderEffectRenderer with an external camera
func camera_transform_changed(camera : Camera):
	var transform = camera.global_transform
	camera_prepass.global_transform = transform
	camera_scene.global_transform = transform


# Call this function to update internal cameras with parameters
# from external camera. 
func set_camera_parameters(camera : Camera):
	# No need to update the following camera parameters:
	# cull_mask as handled by separate functions
	# current, doppler_tracking as this dosn't affect the rendering
	
	# Update parameters effecting the rendering
	camera_prepass.far = camera.far
	camera_scene.far = camera.far
	
	camera_prepass.fov = camera.fov
	camera_scene.fov = camera.fov
	
	camera_prepass.frustum_offset = camera.frustum_offset
	camera_scene.frustum_offset = camera.frustum_offset
	
	camera_prepass.h_offset = camera.h_offset
	camera_scene.h_offset = camera.h_offset
	
	camera_prepass.keep_aspect = camera.keep_aspect
	camera_scene.keep_aspect = camera.keep_aspect
	
	camera_prepass.near = camera.near
	camera_scene.near = camera.near
	
	camera_prepass.projection = camera.projection
	camera_scene.projection = camera.projection
	
	camera_prepass.size = camera.size
	camera_scene.size = camera.size
	
	camera_prepass.v_offset = camera.v_offset
	camera_scene.v_offset = camera.v_offset


# Callback to receive the current camera transform
func _on_camera_transform_changed(camera : Camera):
	camera_transform_changed(camera)
