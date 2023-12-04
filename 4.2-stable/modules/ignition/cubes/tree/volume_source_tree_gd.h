
#ifndef __VOLUME_SOURCE_TREE_GD_H_
#define __VOLUME_SOURCE_TREE_GD_H_

#include "volume_source_gd.h"
#include "volume_source_tree.h"

namespace Ign
{

class VolumeSourceTreeGd: public VolumeSourceGd
{
    GDCLASS(VolumeSourceTreeGd, VolumeSourceGd);
protected:
    static void _bind_methods();

public:
    VolumeSourceTreeGd();
    virtual ~VolumeSourceTreeGd();

    void set_max_depth( int d );
    int  get_max_depth() const;

    void set_max_items_per_node( int qty );
    int  get_max_items_per_node() const;

    virtual real_t value( const Vector3 & at );

    void clear();
    void add_source( const Ref<VolumeSourceGd> & source );

    void subdivide( real_t total_max_size=-1.0 );
    int nodes_qty() const;

public:
	VolumeSourceTree tree;
};


}








#endif




