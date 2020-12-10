
#ifndef __PBD2_CONTACT_POINT_NODE_H_
#define __PBD2_CONTACT_POINT_NODE_H_

#include "scene/3d/spatial.h"

namespace Pbd
{

class PbdContactPointNode: public Spatial
{
    GDCLASS( PbdContactPointNode, Spatial );
public:
    PbdContactPointNode();
    ~PbdContactPointNode();
    
    String get_configuration_warning() const;
    
protected:
    void _notification( int p_what );
    static void _bind_methods();
};

}





#endif






