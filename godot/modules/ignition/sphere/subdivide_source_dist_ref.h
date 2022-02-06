
#ifndef __SUBDIVIDE_SOURCE_DIST_REF_H_
#define __SUBDIVIDE_SOURCE_DIST_REF_H_

#include "subdivide_source_ref.h"
#include "subdivide_source_dist.h"

namespace Ign
{

class SubdivideSourceDistRef: public SubdivideSourceRef
{
    GDCLASS(SubdivideSourceDistRef, SubdivideSourceRef);
    OBJ_CATEGORY("Ignition");
protected:
    static void _bind_methods();

public:
    SubdivideSourceDistRef();
    virtual ~SubdivideSourceDistRef();

	void set_min_level( int lvl );
	int get_min_level() const;

	void set_min_size( real_t sz );
	real_t get_min_size() const;

	void set_min_angle( real_t angle );
	real_t get_min_angle() const;

public:
    SubdivideSourceDist subdivide_source_dist;
};



}





#endif



