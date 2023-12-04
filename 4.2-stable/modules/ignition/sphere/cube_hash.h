
#ifndef __CUBE_HASH_H_
#define __CUBE_HASH_H_

extern "C"
{
#include "hash_md5.h"
}

#include <cinttypes>

namespace Ign
{

class CubeHash
{
public:
    CubeHash();
    ~CubeHash();

    CubeHash( const CubeHash & inst );
    const CubeHash & operator=( const CubeHash & inst );

    void reset( uint64_t init=0 );
    const CubeHash & operator<<( int v );

    uint64_t state() const;


    MD5_CTX ctx_;
};


}


#endif




