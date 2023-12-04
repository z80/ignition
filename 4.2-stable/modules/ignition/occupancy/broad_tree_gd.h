
#include "core/object/ref_counted.h"
//#include "scene/main/node.h"
#include "broad_tree.h"


namespace Ign
{

class OctreeMeshGd;
class RefFrameNode;

class BroadTreeGd: public RefCounted
{
	GDCLASS(BroadTreeGd, RefCounted);
	//OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	BroadTreeGd();
	~BroadTreeGd();

	// Manipulating filling up the occupancy grid.
	void set_max_depth( int new_level=5 );
	int  get_max_depth() const;

	void clear();

	// These three are supposed to be used in simulation loop.
	// "subdivide" should be called once.
	bool subdivide( Node * ref_frame_physics );

	// For visualization.
	//Array lines_nodes( RefFrameNode * camera ) const;

	int  get_octree_meshes_qty() const;
	Node * get_octree_mesh( int ind );

	bool intersects_segment( const Vector3 & start, const Vector3 & end, Node * exclude_mesh ) const;
	Array intersects_segment_face( const Vector3 & start, const Vector3 & end, Node * exclude_mesh ) const;

	Array face_lines( const Transform3D & t_to_cam ) const;
public:
	BroadTree _broad_tree;
};




}



