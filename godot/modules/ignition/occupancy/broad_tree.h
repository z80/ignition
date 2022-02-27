
#ifndef __PBD_BROAD_TREE_H_
#define __PBD_BROAD_TREE_H_

#include "data_types.h"
#include "se3.h"
#include "broad_tree_node.h"

#include "core/variant.h"

namespace Ign
{

class RefFrameNode;
class OctreeMeshGd;

class BroadTree
{
public:
    BroadTree();
    ~BroadTree();

    // Manipulating filling up the occupancy grid.
    void set_max_depth( int new_level=5 );
    int  get_max_depth() const;

    void clear();

    // These three are supposed to be used in simulation loop.
    // "subdivide" should be called once.
    void subdivide( RefFrameNode * ref_frame_physics );

    // For visualization.
    //PoolVector3Array lines_nodes( RefFrameNode * camera ) const;
	void face_lines( Vector<Vector3> & ret ) const;

    // These three for tree construction.
    bool parent( const BroadTreeNode & node, BroadTreeNode * & parent );
    int  insert_node( BroadTreeNode & node );
    void update_node( const BroadTreeNode & node );

    int            get_octree_meshes_qty() const;
    OctreeMeshGd * get_octree_mesh( int ind );

	OctreeMeshGd * intersects_segment( const Vector3 & start, const Vector3 & end, OctreeMeshGd * exclude_mesh ) const;
	OctreeMeshGd * intersects_segment_face( const Vector3 & start, const Vector3 & end, real_t & dist, Vector3 & at, OctreeMesh::FaceProperties & face_props, OctreeMeshGd * exclude_mesh ) const;

    Vector<OctreeMeshGd *> octree_meshes_;
    Vector<BroadTreeNode>  nodes_;

    RefFrameNode * ref_frame_physics_;

    // Maximum subdivision level.
    int    max_depth_;
};


}






#endif





