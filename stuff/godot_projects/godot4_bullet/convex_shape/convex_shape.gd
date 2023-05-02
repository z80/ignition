extends Node3D

@export var mesh: Mesh = null
@export var shape_path: NodePath = ""

# Called when the node enters the scene tree for the first time.
func _ready():
	var shape: CollisionShape3D = get_node( shape_path )
	
	var sh: ConcavePolygonShape3D = ConcavePolygonShape3D.new()
	var arr: PackedVector3Array = mesh.get_faces()
	sh.set_faces( arr )
	#sh.backface_collision = true
	
	shape.shape = sh


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
