extends ImmediateGeometry

export(NodePath) var mm_path setget set_mm_path
var _mm = null

var _m  = SpatialMaterial.new()


func set_mm_path( p ):
	mm_path = p
	_mm = get_node( p )


# Called when the node enters the scene tree for the first time.
func _ready():
	set_mm_path( mm_path )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	apply_mm()




func apply_mm():
	if not _mm:
		return
	
	set_material_override( _m )
	clear()
	begin( Mesh.PRIMITIVE_LINES )
	
	var parents: Array = _mm.PARENTS
	var fr = _mm._f
	
	# Drawing wireframe skeleton.
	print( "" )
	var qty: int = parents.size()
	for i in range( qty ):
		var i_n: int = i*7
		var rb: Vector3 = Vector3( fr[i_n+4], fr[i_n+5], fr[i_n+6] )
		print( " at[", i, "]: ", rb )
	
	for i in range( qty ):
		var i_p: int = parents[i]*7
		if i_p < 0:
			continue
		var i_n: int = i*7
		var ra: Vector3 = Vector3( fr[i_p+4], fr[i_p+5], fr[i_p+6] )
		var rb: Vector3 = Vector3( fr[i_n+4], fr[i_n+5], fr[i_n+6] )
		#var ra: Vector3 = Vector3( fr[i_p+4], fr[i_p+5], fr[i_p+6] )
		#var rb: Vector3 = Vector3( fr[i_n+4], fr[i_n+5], fr[i_n+6] )
		
		add_vertex( ra )
		add_vertex( rb )
	
	# Drawing user input.
	#qty = mm_.vis_control_sequence_.size()
	#for i in range( qty-1 ):
	#	var ra: Vector3 = mm_.vis_control_sequence_[i]
	#	var rb: Vector3 = mm_.vis_control_sequence_[i+1]
	#	ra = Vector3( ra.x, ra.z, -ra.y )
	#	rb = Vector3( rb.x, rb.z, -rb.y )
	#	add_vertex( ra )
	#	add_vertex( rb )
	
	end()




