extends Spatial


export(Vector3) var velocity  = Vector3( 1.0, 0.0, 0.0 )
export(float)   var distance  = 1.0
export(float)   var side_step = 0.2


var broad_tree: BroadTreeGd = null

var shock_stripes: Array = []


func _init():
	broad_tree = BroadTreeGd.new()


# Called when the node enters the scene tree for the first time.
func _ready():
	var body: RefFrameNode = get_node( "RefFrame/Body" )
	var otree_mesh: OctreeMeshGd = get_node( "RefFrame/Body/OctreeMeshGd" )
	otree_mesh.rebuild()
	
	shock_stripes.push_back( get_node( "ShockStripe" ) )
	shock_stripes.push_back( get_node( "ShockStripe_2" ) )
	shock_stripes.push_back( get_node( "ShockStripe_3" ) )
	shock_stripes.push_back( get_node( "ShockStripe_4" ) )
	shock_stripes.push_back( get_node( "ShockStripe_5" ) )
	shock_stripes.push_back( get_node( "ShockStripe_6" ) )
	shock_stripes.push_back( get_node( "ShockStripe_7" ) )
	shock_stripes.push_back( get_node( "ShockStripe_8" ) )
	shock_stripes.push_back( get_node( "ShockStripe_9" ) )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var ref_frame: RefFrameNode = get_node( "RefFrame" )
	var ok: bool = broad_tree.subdivide( ref_frame )
	
	var body: RefFrameNode = get_node( "RefFrame/Body" )
	var otree_mesh: OctreeMeshGd = get_node( "RefFrame/Body/OctreeMeshGd" )
	var body_se3: Se3Ref = body.get_se3()
	# First detect central collision.
	var ret: Array = broad_tree.intersects_segment_face( body_se3.r, velocity * 10.0, null )
	
	#print( ret )
	
	draw_shock_wave()




func draw_shock_wave():
#	for ss in shock_stripes:
#		var shock: Node = ss
#		var otree_mesh: OctreeMeshGd = get_node( "RefFrame/Body/OctreeMeshGd" )
#		shock.apply( velocity, broad_tree, [otree_mesh] )


	var shock: Node = get_node( "ShockWave" )
	var otree_mesh: OctreeMeshGd = get_node( "RefFrame/Body/OctreeMeshGd" )
	shock.apply( velocity, broad_tree, [otree_mesh] )





