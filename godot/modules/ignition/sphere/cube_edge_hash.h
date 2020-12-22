
#ifndef __CUBE_EDGE_HASH_H_
#define __CUBE_EDGE_HASH_H_

namespace Ign
{

static const int EDGE_HASH_SZ = sizeof(int)*2;

class EdgeHash
{
public:
    unsigned char d[EDGE_HASH_SZ];

    EdgeHash();
    ~EdgeHash();
    EdgeHash( int a, int b );
    EdgeHash( const EdgeHash & inst );
    const EdgeHash & operator=( const EdgeHash & inst );
    //unsigned ToHash() const;
    friend bool operator<( const EdgeHash & a, const EdgeHash & b );
    friend bool operator==( const EdgeHash & a, const EdgeHash & b );
};



}



#endif




