
#ifndef __DETERMINISTIC_SOURCE_0_H_
#define __DETERMINISTIC_SOURCE_0_H_

#include "deterministic_source.h"

namespace Ign
{

class DeterministicSource0: public DeterministicSource
{
public:
    DeterministicSource0( const PiSourceDesc & body );
    ~DeterministicSource0();

    Float height( const Vector3d & at ) const override;
    Color color( const Vector3d & at, const Vector3d & norm, Float height )  const override;

public:
};

}

#endif



