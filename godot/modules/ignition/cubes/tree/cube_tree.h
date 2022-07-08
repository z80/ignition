
#ifndef __CUBE_TREE_H_
#define __CUBE_TREE_H_

#include "data_types.h"
#include "se3.h"
#include "cube_tree_node.h"

#include <vector>

namespace Ign
{

class MarchingVolumeObject;

class CubeTree
{
public:
    CubeTree();
    virtual ~CubeTree();

    void set_max_depth( int d );
    int  get_max_depth() const;

    void set_max_items_per_node( int qty );
    int  get_max_items_per_node() const;

    virtual void fill_source_references();

    const std::vector<const MarchingVolumeObject *> & pick_objects( const Vector3d & at );

    // These three are supposed to be used in simulation loop.
    // "subdivide" should be called once.
    void subdivide( Float total_max_size=-1.0 );

    // Manipulating filling up the occupancy grid.
    void compute_levels( Float total_max_size );

    // These three for tree construction.
    bool parent( const CubeTreeNode & node, CubeTreeNode * & parent );
    int  insert_node( CubeTreeNode & node );
    void update_node( const CubeTreeNode & node );

    Vector3d at( const VectorInt & at_i ) const;

    void clear();
    void add_source( MarchingVolumeObject * source );


    std::vector<MarchingVolumeObject *> sources;
    std::vector<int>                    query_result_inds;
    std::vector<const MarchingVolumeObject *> query_result;
    std::vector<CubeTreeNode>           nodes;

    // Maximum subdivision level.
    int   max_depth;
    int   max_items_per_node;
    Float step;
};


}






#endif






