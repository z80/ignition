
extends RefFrameRotationNode

export(PackedScene) var collision_surface_scene = null
var _visual_surface: Node = null


var _collision_surfaces: Dictionary = {}


# Called when the node enters the scene tree for the first time.
func _ready():
	_collision_surfaces = {}
	_create_collision_surfaces()


func _child_jumped( child_ref_frame: RefFrameNode ):
	var phys: RefFramePhysics = child_ref_frame as RefFramePhysics
	if phys == null:
		return
	
	#var has: bool = _collision_surfaces.has( phys )
	#assert( has )
	var collision_surface: Node = _collision_surfaces[phys]
	var all_surfs: Node = get_child(0)
	var surface_source: Resource = all_surfs.get_surface_source()
	
	print( "Child jumped" )
	# Immediately query faces using old subdivision.
	# It is done inside "rebuild_surface" anyway.
	#collision_surface.update_surface( phys, self, surface_source )
	# Asynchronously rebuild surface.
	collision_surface.rebuild_surface( phys, self, surface_source )



func _child_entered( child_ref_frame: RefFrameNode ):
	var phys: RefFramePhysics = child_ref_frame as RefFramePhysics
	if phys == null:
		return
	
	var has: bool = _collision_surfaces.has( phys )
	assert( not has )

	var all_surfs: Node = get_child(0)
	var surface_source: Resource = all_surfs.get_surface_source()

	var env: Node = phys.get_physics_environment()
	var collision_surf: Node = collision_surface_scene.instance()
	
	env.add_child( collision_surf )
	collision_surf.rebuild_surface( phys, self, surface_source )
	
	_collision_surfaces[phys] = collision_surf




func _child_left( child_ref_frame: RefFrameNode ):
	var phys: RefFramePhysics = child_ref_frame as RefFramePhysics
	if phys == null:
		return

	var has: bool = _collision_surfaces.has( phys )
	assert( has )

	var collision_surface: Node = _collision_surfaces[phys]
	collision_surface.queue_free()




func _create_collision_surfaces():
	#return
	
	var visual_surface: Node = _get_visual_surface()
	var surface_source: Resource = visual_surface.surface_source_solid
	
	var qty: int = self.get_child_count()
	for i in range(qty):
		var ch: Node = self.get_child(i)
		var ref_frame_physics: RefFramePhysics = ch as RefFramePhysics
		if ref_frame_physics == null:
			continue
		
		var env: Node = ref_frame_physics.get_physics_environment()
		var collision_surf: Node = collision_surface_scene.instance()
		collision_surf.surface_source = surface_source
		
		env.add_physics_body( collision_surf )
		collision_surf.rebuild_surface( ref_frame_physics, self, surface_source )
		
		_collision_surfaces[ref_frame_physics] = collision_surf


func _get_visual_surface():
	if _visual_surface == null:
		_visual_surface = get_node( "VisualSurface" )
	
	return _visual_surface





