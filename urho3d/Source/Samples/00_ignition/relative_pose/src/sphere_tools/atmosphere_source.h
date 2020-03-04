
#ifndef __ATMOSPHERE_SOURCE_H_
#define __ATMOSPHERE_SOURCE_H_

#include "data_types.h"

namepsace Ign
{

class AtmosphereSource
{
public:
    AtmosphereSource();
    virtual ~AtmosphereSource();

    bool params( Float height, Float & pressure, Float & density ) const;
    bool drag( PhysicsItem * b, const State & st, Vector3d & F, Vector3d & P );

public:
    Float normalCoef_, 
          lateralCoef_;
};

}



#endif









