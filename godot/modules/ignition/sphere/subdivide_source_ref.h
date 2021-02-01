
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

    void clear_levels();
    void add_level( Float sz, Float dist );

    bool need_subdivide( Node * ref_frame, Node * cubesphere );

public:
    SubdivideSource subdivide_source;
};



}





#endif



