
#include "cube_Edge_hash.h"

namespace Ign
{


EdgeHash::EdgeHash()
{
    for ( int i=0; i<EDGE_HASH_SZ; i++ )
        d[i] = 0;
}

EdgeHash::~EdgeHash()
{

}

EdgeHash::EdgeHash( int a, int b )
{
    unsigned char * pa, * pb;
    if ( a <= b )
    {
        pa = reinterpret_cast<unsigned char *>( &a );
        pb = reinterpret_cast<unsigned char *>( &b );
    }
    else
    {
        pa = reinterpret_cast<unsigned char *>( &b );
        pb = reinterpret_cast<unsigned char *>( &a );
    }
    const int sz = sizeof(int);
    int ind = 0;
    for ( int i=0; i<sz; i++ )
        d[ind++] = pa[i];
    for ( int i=0; i<sz; i++ )
        d[ind++] = pb[i];
}

EdgeHash::EdgeHash( const EdgeHash & inst )
{
    *this = inst;
}

const EdgeHash & EdgeHash::operator=( const EdgeHash & inst )
{
    if ( this != &inst )
    {
        for ( int i=0; i<EDGE_HASH_SZ; i++ )
            d[i] = inst.d[i];
    }
    return *this;
}

/*unsigned EdgeHash::ToHash() const
{
    long long a = 0;
    for ( int i=0; i<EDGE_HASH_SZ; i++ )
        a = (a << 8) + d[i];
    const unsigned h = MakeHash( a );
    return h;
}*/

bool operator<( const EdgeHash & a, const EdgeHash & b )
{
    for ( int i=0; i<EDGE_HASH_SZ; i++ )
    {
        if ( a.d[i] < b.d[i] )
            return true;
        else if ( a.d[i] > b.d[i] )
            return false;
    }
    return false;
}

bool operator==( const EdgeHash & a, const EdgeHash & b )
{
    for ( int i=0; i<EDGE_HASH_SZ; i++ )
    {
        if ( a.d[i] != b.d[i] )
            return false;
    }
    return true;
}




}




