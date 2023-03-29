
extends RefFrameRotationNode

var _visual_surface: Node = null



# Called when the node enters the scene tree for the first time.
func _ready():
	call_deferred( "_create_collision_surfaces" )


func _child_jumped( child_ref_frame: RefFrameNode ):
	var phys: RefFramePhysics = child_ref_frame as RefFramePhysics
	if phys == null:
		return
	
	#var has: bool = _collision_surfaces.has( phys )
	#assert( has )
	var collision_surface: Node = phys.get_collision_surface()
	var visual_surface: Node = _get_visual_surface()
	var surface_source: Resource = visual_surface.surface_source
	
	print( "Child jumped" )
	# Immediately query faces using old subdivision.
	# It is done inside "rebuild_surface" anyway.
	#collision_surface.update_surface( phys, self, surface_source )
	# Asynchronously rebuild surface.
	collision_surface.rebuild_surface( surface_source, false )



func _child_entered( child_ref_frame: RefFrameNode ):
	var ref_frame_physics: RefFramePhysics = child_ref_frame as RefFramePhysics
	if ref_frame_physics == null:
		return

	var collision_surface: Node = ref_frame_physics.get_collision_surface()
	var visual_surface: Node = _get_visual_surface()
	var surface_source: Resource = visual_surface.surface_source_solid

	collision_surface.rebuild_surface( surface_source, false )
	




func _child_left( child_ref_frame: RefFrameNode ):
	var ref_frame_physics: RefFramePhysics = child_ref_frame as RefFramePhysics
	if (ref_frame_physics == null) or (not is_instance_valid(ref_frame_physics)):
		return

	var collision_surface: Node = ref_frame_physics.get_collision_surface()
	collision_surface.remove_surface()




func _create_collision_surfaces():
	#return
	
	var visual_surface: Node = _get_visual_surface()
	var surface_source: Resource = visual_surface.surface_source
	
	var qty: int = self.get_child_count()
	for i in range(qty):
		var ch: Node = self.get_child(i)
		var ref_frame_physics: RefFramePhysics = ch as RefFramePhysics
		if ref_frame_physics == null:
			continue
		
		var collision_surface: Node = ref_frame_physics.get_collision_surface()
		collision_surface.rebuild_surface( surface_source, true )



func _get_visual_surface():
	if _visual_surface == null:
		_visual_surface = get_node( "VisualSurface" )
	
	return _visual_surface



func on_deserialize():
	call_deferred( "_create_collision_surfaces" )

