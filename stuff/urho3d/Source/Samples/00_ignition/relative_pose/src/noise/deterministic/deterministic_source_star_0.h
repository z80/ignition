
#ifndef __DETERMINISTIC_SOURCE_STAR_0_H_
#define __DETERMINISTIC_SOURCE_STAR_0_H_

#include "deterministic_source.h"

namespace Ign
{

class DeterministicSourceStar0: public DeterministicSource
{
public:
    DeterministicSourceStar0( const PiSourceDesc & body );
    ~DeterministicSourceStar0();

    Float height( const Vector3d & at ) const override;
    Color color( const Vector3d & at, const Vector3d & norm, Float height )  const override;

public:
};

}

#endif



