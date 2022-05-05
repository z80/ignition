
#ifndef __VOLUME_SOURCE_TREE_H_
#define __VOLUME_SOURCE_TREE_H_

#include "cube_tree.h"
#include "volume_source_gd.h"

namespace Ign
{

class VolumeSourceTree: public VolumeSource
{
public:
    typedef Ref<VolumeSourceGd> Src;

    VolumeSourceTree();
    virtual ~VolumeSourceTree();

    void set_max_depth( int d );
    int  get_max_depth() const;

    void set_max_items_per_node( int qty );
    int  get_max_items_per_node() const;

    virtual Float value( const Vector3d & at );

    void clear();
    void add_source( Src & source );

    void subdivide( Float total_max_size=-1.0 );
    int nodes_qty() const;

public:
    CubeTree    tree;
    Vector<Src> sources;
};

}





#endif



