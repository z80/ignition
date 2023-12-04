
#ifndef __SUBDIVIDE_SOURCE_REF_H_
#define __SUBDIVIDE_SOURCE_REF_H_

#include "subdivide_source.h"
#include "core/object/ref_counted.h"
#include "scene/main/node.h"

namespace Ign
{

class SubdivideSourceRef: public RefCounted
{
    GDCLASS(SubdivideSourceRef, RefCounted);
    //OBJ_CATEGORY("Ignition");
protected:
    static void _bind_methods();

public:
    SubdivideSourceRef();
    virtual ~SubdivideSourceRef();

    bool need_subdivide( Node * ref_frame, Node * cubesphere_node );
	void force_subdivide();

public:
	SubdivideSource * source;
    SubdivideSource subdivide_source;
    Vector<Vector3d> subdivide_points;
};



}





#endif



