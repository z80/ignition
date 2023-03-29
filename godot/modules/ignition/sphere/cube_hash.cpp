
#include "cube_hash.h"

namespace Ign
{

CubeHash::CubeHash()
{
    reset();
}

CubeHash::~CubeHash()
{
}

CubeHash::CubeHash( const CubeHash & inst )
{
    *this = inst;
}

const CubeHash & CubeHash::operator=( const CubeHash & inst )
{
    if ( this != &inst )
    {
        ctx_ = inst.ctx_;
    }
    return *this;
}

void CubeHash::reset( uint64_t init )
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

const CubeHash & CubeHash::operator<<( int v )
{
	const unsigned char sign = (v >= 0) ? 0 : 1;
	int abs_v = (v >= 0) ? v : (-v);
	const int sz = sizeof(int);
	const int qty = sz + 1;
	unsigned char cb[qty];
	for ( int i=0; i<sz; i++ )
	{
		cb[i] = abs_v & 0xFF;
		abs_v = abs_v >> 8;
	}
	cb[sz] = sign;

	md5_update( &ctx_, cb, qty );

    return *this;
}

uint64_t CubeHash::state() const
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









