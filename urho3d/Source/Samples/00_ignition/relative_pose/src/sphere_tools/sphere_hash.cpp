
#include "sphere_hash.h"

namespace Ign
{

SphereHash::SphereHash()
{
}

SphereHash::~SphereHash()
{
}

SphereHash::SphereHash( const SphereHash & inst )
{
    *this = inst;
}

const SphereHash & SphereHash::operator=( const SphereHash & inst )
{
    if ( this != &inst )
    {
        ctx_ = inst.ctx_;
    }
    return *this;
}

void SphereHash::reset()
{
    md5_init( &ctx_ );
}

const SphereHash & SphereHash::operator<<( int v )
{
    unsigned char cb[1];
    cb[0] = static_cast<unsigned char>( v );
    md5_update( &ctx_, cb );
    return *this;
}

uint64_t SphereHash::state() const
{

}


}












