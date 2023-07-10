
extends RefFrameRotationNode

var _visual_surface: Node = null
var _ref_frame_rebuild_index: int = 0
var _ref_frame_process_index: int = 0
var _ref_frame_other_index: int = 0


# Called when the node enters the scene tree for the first time.
func _ready():
	call_deferred( "_create_collision_surfaces" )


func _child_jumped( child_ref_frame ):
	var phys: RefFramePhysics = child_ref_frame as RefFramePhysics
	if phys == null:
		return
	
	#var has: bool = _collision_surfaces.has( phys )
	#assert( has )
	var collision_surface: Node = phys.get_collision_surface()
	var visual_surface: Node = _get_visual_surface()
	var surface_source: Resource = visual_surface.surface_source
	var foliage_sources: Array   = visual_surface.foliage_sources
	
	print( "Child jumped" )
	# Immediately query faces using old subdivision.
	# It is done inside "rebuild_surface" anyway.
	#collision_surface.update_surface( phys, self, surface_source )
	# Asynchronously rebuild surface.
	#collision_surface.rebuild_surface( surface_source, false )
	collision_surface.rebuild_surface( surface_source, foliage_sources, true )



func _child_entered( child_ref_frame ):
	var ref_frame_physics: RefFramePhysics = child_ref_frame as RefFramePhysics
	if ref_frame_physics == null:
		return

	var collision_surface: Node = ref_frame_physics.get_collision_surface()
	var visual_surface: Node = _get_visual_surface()
	var surface_source: Resource = visual_surface.surface_source_solid
	var foliage_sources: Array   = visual_surface.foliage_sources

	#collision_surface.rebuild_surface( surface_source, false )
	collision_surface.rebuild_surface( surface_source, foliage_sources, true )
	




func _child_left( child_ref_frame ):
	var ref_frame_physics: RefFramePhysics = child_ref_frame as RefFramePhysics
	if (ref_frame_physics == null) or (not is_instance_valid(ref_frame_physics)):
		return

	var collision_surface: Node = ref_frame_physics.get_collision_surface()
	collision_surface.remove_surface()




func _create_collision_surfaces():
	#return
	
	var visual_surface: Node = _get_visual_surface()
	var surface_source: Resource = visual_surface.surface_source
	var foliage_sources: Array   = visual_surface.foliage_sources
	
	var qty: int = self.get_child_count()
	for i in range(qty):
		var ch: Node = self.get_child(i)
		var ref_frame_physics: RefFramePhysics = ch as RefFramePhysics
		if ref_frame_physics == null:
			continue
		
		var collision_surface: Node = ref_frame_physics.get_collision_surface()
		collision_surface.rebuild_surface( surface_source, foliage_sources, true )



func _get_visual_surface():
	if _visual_surface == null:
		_visual_surface = get_node( "VisualSurface" )
	
	return _visual_surface




func _ign_pre_process(delta):
	# Update collision surfaces for physics ref. frames 
	# one at a time.
	_update_surface()
	_process_ref_frames()



func _update_surface():
	var qty: int = self.get_child_count()
	if _ref_frame_rebuild_index >= qty:
		_ref_frame_rebuild_index = 0
	
	var ch: Node = self.get_child( _ref_frame_rebuild_index )
	_ref_frame_rebuild_index += 1
	var ref_frame_physics: RefFramePhysics = ch as RefFramePhysics
	if ref_frame_physics == null:
		return
	
	var visual_surface: Node     = _get_visual_surface()
	var surface_source: Resource = visual_surface.surface_source
	var foliage_sources: Array   = visual_surface.foliage_sources
	
	var collision_surface: Node = ref_frame_physics.get_collision_surface()
	collision_surface.rebuild_surface( surface_source, foliage_sources, true )






func _process_ref_frames():
	var qty: int = self.get_child_count()
	
	if _ref_frame_other_index >= qty:
		_ref_frame_process_index += 1
		_ref_frame_other_index = _ref_frame_process_index + 1
	
	if _ref_frame_process_index >= qty:
		_ref_frame_process_index = 0
		_ref_frame_other_index = 1
	
	if _ref_frame_other_index <= _ref_frame_process_index:
		_ref_frame_other_index = _ref_frame_process_index + 1
	
	var ch: Node = self.get_child( _ref_frame_process_index )
	var ref_frame_a: RefFramePhysics = ch as RefFramePhysics
	
	if ref_frame_a == null:
		_ref_frame_other_index += 1
		return false
	
	ref_frame_a.process_children_surface()
	
	if _ref_frame_other_index >= qty:
		return false
	
	ch = self.get_child( _ref_frame_other_index )
	_ref_frame_other_index += 1
	var ref_frame_b: RefFramePhysics = ch as RefFramePhysics
	
	if ref_frame_b == null:
		return false
	
	var ret: bool = ref_frame_a.merge_if_needed_surface( ref_frame_b )
	return ret





func on_deserialize():
	call_deferred( "_create_collision_surfaces" )






