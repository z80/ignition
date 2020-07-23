extends ImmediateGeometry

var MotionMatching = preload("res://mm/motion_matching.gd")
# Declare member variables here. Examples:
# var a = 2
# var b = "text"
var mm_ = null
var m_  = SpatialMaterial.new()


# Called when the node enters the scene tree for the first time.
func _ready():
	get_parent_mm_()


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	apply_mm()



func get_parent_mm_():
	var p = get_parent()
	if p is MotionMatching:
		mm_ = p
	else:
		mm_ = null

func apply_mm():
	if not mm_:
		return
	
	set_material_override( m_ )
	clear()
	begin( Mesh.PRIMITIVE_LINES )
	
	var parents: Array = mm_.parents_
	var fr = mm_.f_
	var qty: int = parents.size()
	for i in range( qty ):
		var i_p: int = parents[i]*7
		if i_p < 0:
			continue
		var i_n: int = i*7
		var ra: Vector3 = Vector3( fr[i_p+4], fr[i_p+5], fr[i_p+6] )
		var rb: Vector3 = Vector3( fr[i_n+4], fr[i_n+5], fr[i_n+6] )
		
		add_vertex( ra )
		add_vertex( rb )
	end()
