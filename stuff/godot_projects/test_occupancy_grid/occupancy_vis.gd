extends ImmediateGeometry


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
var verts = null

var m = SpatialMaterial.new()

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if not verts:
		return
	
	var sz: int = verts.size()
	if sz < 2:
		return
	
	set_material_override( m )
	
	var vv = [ Vector3(-5, -5, -5 ), Vector3( 5, 5, 5 ) ]
	
	clear()
	
	begin( Mesh.PRIMITIVE_LINES )
	
	for i in range( 2 ):
		var v = vv[i]
		add_vertex( v )

	for i in range( sz ):
		var v = verts[i] * 5.0
		#if i < 30:
		#	print( v )
		add_vertex( v )
		
	end()
	
	
	
