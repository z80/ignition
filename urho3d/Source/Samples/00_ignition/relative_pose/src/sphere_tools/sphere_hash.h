
#ifndef __SPHERE_HASH_H_
#define __SPHERE_HASH_H_

extern "C"
{
#include "hash_md5.h"
}

namespace Ign
{

class SphereHash
{
public:
    SphereHash();
    ~SphereHash();

    SphereHash( const SphereHash & inst );
    const SphereHash & operator=( const SphereHash & inst );

    void reset();
    const SphereHash & operator<<( int v );


    MD5_CTX ctx_;
};


}





#endif






