
#ifndef __SPHERE_HASH_H_
#define __SPHERE_HASH_H_

extern "C"
{
#include "hash_md5.h"
}

#include <cinttypes>

namespace Ign
{

class SphereHash
{
public:
    SphereHash();
    ~SphereHash();

    SphereHash( const SphereHash & inst );
    const SphereHash & operator=( const SphereHash & inst );

    void reset( uint64_t init=0 );
    const SphereHash & operator<<( int v );

    uint64_t state() const;


    MD5_CTX ctx_;
};


}





#endif






