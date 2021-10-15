
#ifndef __DETERMINISTIC_SOURCE_MOON_H_
#define __DETERMINISTIC_SOURCE_MOON_H_

#include "deterministic_source.h"

namespace Ign
{

class DeterministicSourceMoon: public DeterministicSource
{
public:
    DeterministicSourceMoon( const PiSourceDesc & body );
    ~DeterministicSourceMoon();

    Float height( const Vector3d & at ) const override;
    Color color( const Vector3d & at, const Vector3d & norm, Float height )  const override;

public:
};

}

#endif



