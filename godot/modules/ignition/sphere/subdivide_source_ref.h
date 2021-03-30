
#ifndef __SUBDIVIDE_SOURCE_REF_H_
#define __SUBDIVIDE_SOURCE_REF_H_

#include "subdivide_source.h"
#include "core/reference.h"

namespace Ign
{

class SubdivideSourceRef: public Reference
{
    GDCLASS(SubdivideSourceRef, Reference);
    OBJ_CATEGORY("Ignition");
protected:
    static void _bind_methods();

public:
    SubdivideSourceRef();
    ~SubdivideSourceRef();

    bool need_subdivide( Node * ref_frame, Node * cubesphere_node );
	void force_subdivide();

public:
    SubdivideSource subdivide_source;
    Vector<Vector3d> subdivide_points;
};



}





#endif



