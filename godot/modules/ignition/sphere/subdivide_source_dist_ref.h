
#ifndef __SUBDIVIDE_SOURCE_DIST_REF_H_
#define __SUBDIVIDE_SOURCE_DIST_REF_H_

#include "subdivide_source_dist.h"
#include "core/reference.h"

namespace Ign
{

class SubdivideSourceDistRef: public Reference
{
    GDCLASS(SubdivideSourceDistRef, Reference);
    OBJ_CATEGORY("Ignition");
protected:
    static void _bind_methods();

public:
    SubdivideSourceDistRef();
    ~SubdivideSourceDistRef();

    bool need_subdivide( Node * ref_frame, Node * cubesphere_node );
	void force_subdivide();

public:
    SubdivideSourceDist subdivide_source;
    Vector<Vector3d>    subdivide_points;
};



}





#endif



