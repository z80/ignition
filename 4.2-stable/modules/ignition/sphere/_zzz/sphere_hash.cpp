
#include "sphere_hash.h"

namespace Ign
{

SphereHash::SphereHash()
{
    reset();
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

void SphereHash::reset( uint64_t init )
{
    md5_init( &ctx_ );

    unsigned char cb[8];
    for ( int i=0; i<8; i++ )
    {
	    cb[i] = static_cast<unsigned char>(init & 0x255);
	    init = init >> 8;
    }
    md5_update( &ctx_, cb, 8 );
}

const SphereHash & SphereHash::operator<<( int v )
{
    unsigned char cb[1];
    cb[0] = static_cast<unsigned char>( v );
    md5_update( &ctx_, cb, 1 );
    return *this;
}

uint64_t SphereHash::state() const
{
    unsigned char hash[MD5_BLOCK_SIZE];
    md5_final( const_cast<MD5_CTX *>( &ctx_ ), hash );
    uint64_t v =  static_cast<uint64_t>( hash[0] ) | 
	             (static_cast<uint64_t>( hash[1] ) << 8) | 
		         (static_cast<uint64_t>( hash[2] ) << 16) | 
		         (static_cast<uint64_t>( hash[3] ) << 24) | 
                 (static_cast<uint64_t>( hash[4] ) << 32) | 
		         (static_cast<uint64_t>( hash[5] ) << 40) | 
		         (static_cast<uint64_t>( hash[6] ) << 48) | 
                 (static_cast<uint64_t>( hash[7] ) << 56);
    return v;
}


}












