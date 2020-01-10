
#include "cube_sphere.h"

namespace Ign
{

Vertex::Vertex()
{
    a = b = -1;
    leafFacesQty = 0;
    isMidPoint   = false;
}

Vertex::~Vertex()
{

}

Vertex::Vertex( const Vector3d & at )
{
    a = b = -1;
    leafFacesQty = 0;
    isMidPoint   = false;
    this->at = at;
}

Vertex::Vertex( const Vertex & inst )
{
    *this = inst;
}

const Vertex & Vertex::operator=( const Vertex & inst )
{
    if ( this != &inst )
    {
        at   = inst.at;
        norm = inst.norm;
        a    = inst.a;
        b    = inst.b;
        leafFacesQty = inst.leafFacesQty;
        isMidPoint   = inst.isMidPoint;
    }
    return *this;
}


Face::Face()
{
    for ( int i=0; i<4; i++ )
    {
        vertexInds[i] = -1;
        childInds[i]  = -1;
    }
    parentInd     = -1;
    indexInParent = -1;
    level = -1;
    leaf  = true;
}

Face::~Face()
{
}

Face::Face( int a, int b, int c, int d )
{
    vertexInds[0] = a;
    vertexInds[1] = b;
    vertexInds[2] = c;
    vertexInds[3] = d;

    for ( int i=0; i<4; i++ )
        childInds[i]  = -1;
    parentInd     = -1;
    indexInParent = -1;
    level = -1;
    leaf  = true;
}

Face::Face( const Face & inst )
{
    *this = inst;
}

const Face & Face::operator=( const Face & inst )
{
    if ( this != &inst )
    {
        for ( int i=0; i<4; i++ )
        {
            vertexInds[i] = inst.vertexInds[i];
            childInds[i]  = inst.childInds[i];
        }
        parentInd     = inst.parentInd;
        indexInParent = inst.indexInParent;
        level         = inst.level;
        leaf          = inst.leaf;
    }
    return *this;
}

bool Face::subdrive(Cubesphere * s, Source * src )
{
    if ( !leaf )
    {
        for ( int i=0; i<4; i++ )
        {
            const int faceInd = childInds[i];
            Face f = s->faces[faceInd];
            const bool ok = f.subdrive( s, src );
            s->faces[faceInd] = f;
            if ( !ok )
                return false;
            return true;
        }
    }

    // Check if needs subdivision.
    const bool needProcessing = src->needSubdrive( s, this );
    if ( !needProcessing )
        return true;

    const int newLevel = this->level + 1;
    this->leaf = false;

    int newVertInds[5];
    for ( int i=0; i<4; i++ )
    {
        const int indA = i;
        int indB = i+1;
        if ( indB > 3 )
            indB = 0;
        const int vertIndA = vertexInds[indA];
        const int vertIndB = vertexInds[indB];
        const EdgeHash hashN( vertIndA, vertIndB );
        std::map<EdgeHash, int>::const_iterator it = s->lookup.find( hashN );
        if ( it == s->lookup.end() )
        {
            const Vertex & va = s->verts[vertIndA];
            const Vertex & vb = s->verts[vertIndB];
            // Create new vertex;
            Vector3d at( (va.at + vb.at) * 0.5 );
            Vertex v;
            v.at = at;
            v.a = vertIndA;
            v.b = vertIndB;
            const int newInd = (int)s->verts.Size();
            s->verts.Push( v );
            s->lookup.insert( std::pair<EdgeHash, int>( hashN, newInd ) );
            newVertInds[i] = newInd;
        }
        else
        {
            const int vertInd = it->second;
            //Vertex & v = s->verts[vertInd];
            newVertInds[i] = vertInd;
        }
    }
    // Also create middle vertex.
    const Vertex & a = s->verts[newVertInds[0]];
    const Vertex & b = s->verts[newVertInds[1]];
    const Vertex & c = s->verts[newVertInds[2]];
    const Vertex & d = s->verts[newVertInds[3]];

    Vector3d at( (a.at + b.at + c.at + d.at)*0.25 );
    Vertex v;
    v.at = at;
    v.a = -1;
    v.b = -1;
    const int fifthVertInd = (int)s->verts.Size();
    s->verts.Push( v );
    newVertInds[4] = fifthVertInd;

    // Create faces.
    Face fa( vertexInds[0],  newVertInds[0], newVertInds[4], newVertInds[3] );
    Face fb( newVertInds[0],  vertexInds[1], newVertInds[1], newVertInds[4] );
    Face fc( newVertInds[4], newVertInds[1],  vertexInds[2], newVertInds[2] );
    Face fd( newVertInds[3], newVertInds[4], newVertInds[2],  vertexInds[3] );
    fa.level = newLevel;
    fa.leaf  = true;
    fa.indexInParent = 0;

    fb.level = newLevel;
    fb.leaf  = true;
    fb.indexInParent = 1;

    fc.level = newLevel;
    fc.leaf  = true;
    fc.indexInParent = 2;

    fd.level = newLevel;
    fd.leaf  = true;
    fd.indexInParent = 3;

    int faceIndBase = s->faces.Size();
    const int indBase = faceIndBase;

    s->faces.Push( fa );
    s->faces.Push( fb );
    s->faces.Push( fc );
    s->faces.Push( fd );

    this->childInds[0] = faceIndBase++;
    this->childInds[1] = faceIndBase++;
    this->childInds[2] = faceIndBase++;
    this->childInds[3] = faceIndBase;

    // Recursively subdrive.
    for ( int i=0; i<4; i++ )
    {
        const int faceInd = indBase + i;
        Face face = s->faces[faceInd];
        const bool ok = face.subdrive( s, src );
        s->faces[faceInd] = face;
        if ( !ok )
            return false;
    }

    return true;
}

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















Cubesphere::Cubesphere()
{
    verts.Reserve( 4096 );
    faces.Reserve( 4096 );
}

Cubesphere::~Cubesphere()
{

}

Cubesphere::Cubesphere( const Cubesphere & inst )
{
    *this = inst;
}

const Cubesphere & Cubesphere::operator=( const Cubesphere & inst )
{
    if ( this != &inst )
    {
        verts  = inst.verts;
        faces  = inst.faces;
        lookup = inst.lookup;
    }

    return *this;
}

bool Cubesphere::subdrive( Source * src )
{
    clear();
    init();

    const int qty = (int)faces.Size();
    for ( int i=0; i<qty; i++ )
    {
        Face f = faces[i];
        if ( !f.leaf )
            continue;
        const bool ok = f.subdrive( this, src );
        faces[i] = f;
        if ( !ok )
            return false;
    }

    labelMidPoints();
    scaleToSphere();
    // Here might not be needed.
    //computeNormals();
    applySource( src );

    return true;
}

void Cubesphere::triangleList( Vector<Vertex> & tris )
{
    tris.Clear();
    const unsigned qty = faces.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        const Face & f = faces[i];
        if ( f.leaf )
        {
            const int ind0 = f.vertexInds[0];
            const int ind1 = f.vertexInds[1];
            const int ind2 = f.vertexInds[2];
            const int ind3 = f.vertexInds[3];
            tris.Push( this->verts[ind0] );
            tris.Push( this->verts[ind1] );
            tris.Push( this->verts[ind2] );
            tris.Push( this->verts[ind0] );
            tris.Push( this->verts[ind2] );
            tris.Push( this->verts[ind3] );
        }
    }
}

void Cubesphere::clear()
{
    verts.Clear();
    faces.Clear();
    lookup.clear();
}

void Cubesphere::applySource( Source * src )
{
    const int qty = (int)verts.Size();
    for ( int i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        if ( !v.isMidPoint )
            applySource( src, v );
    }

    for ( int i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        if ( v.isMidPoint )
        {
            const int vertIndA = v.a;
            const int vertIndB = v.b;
            const Vertex & va = verts[vertIndA];
            const Vertex & vb = verts[vertIndB];
            v.at = (va.at + vb.at) * 0.5;
            v.norm = va.norm + vb.norm;
            v.norm.Normalize();
        }
    }

    computeNormals();
}

void Cubesphere::init()
{
    Vertex v;
    v.at = Vector3d( -1.0, -1.0, -1.0 );
    verts.Push( v );

    v.at = Vector3d( -1.0,  1.0, -1.0 );
    verts.Push( v );

    v.at = Vector3d(  1.0,  1.0, -1.0 );
    verts.Push( v );

    v.at = Vector3d(  1.0, -1.0, -1.0 );
    verts.Push( v );

    v.at = Vector3d( -1.0, -1.0,  1.0 );
    verts.Push( v );

    v.at = Vector3d( -1.0,  1.0,  1.0 );
    verts.Push( v );

    v.at = Vector3d(  1.0,  1.0,  1.0 );
    verts.Push( v );

    v.at = Vector3d(  1.0, -1.0,  1.0 );
    verts.Push( v );

    Face f;
    f.leaf  = true;
    f.level = 0;
    f.indexInParent = -1;
    f.childInds[0] = -1;
    f.childInds[1] = -1;
    f.childInds[2] = -1;
    f.childInds[3] = -1;

    f.vertexInds[0] = 0;
    f.vertexInds[1] = 1;
    f.vertexInds[2] = 2;
    f.vertexInds[3] = 3;
    faces.Push( f );

    f.vertexInds[0] = 4;
    f.vertexInds[1] = 5;
    f.vertexInds[2] = 1;
    f.vertexInds[3] = 0;
    faces.Push( f );

    f.vertexInds[0] = 7;
    f.vertexInds[1] = 6;
    f.vertexInds[2] = 5;
    f.vertexInds[3] = 4;
    faces.Push( f );

    f.vertexInds[0] = 3;
    f.vertexInds[1] = 2;
    f.vertexInds[2] = 6;
    f.vertexInds[3] = 7;
    faces.Push( f );

    f.vertexInds[0] = 1;
    f.vertexInds[1] = 5;
    f.vertexInds[2] = 6;
    f.vertexInds[3] = 2;
    faces.Push( f );

    f.vertexInds[0] = 4;
    f.vertexInds[1] = 0;
    f.vertexInds[2] = 3;
    f.vertexInds[3] = 7;
    faces.Push( f );


    // Debugging.
    /*{
        const unsigned qty = verts.Size();
        for ( unsigned i=0; i<qty; i++ )
        {
            Vertex & v = verts[i];
            v.at = v.at * 0.001 + Vector3d( 0.0, 1.0, 0.0 );
        }
    }*/
}

void Cubesphere::labelMidPoints()
{
    // Reset numebr of leaf faces it belongs to.
    const int qty = (int)verts.Size();
    for ( int i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        v.leafFacesQty = 0;
        v.isMidPoint   = false;
    }

    // Count number of leaf faces it belongs to.
    const int facesQty = faces.Size();
    for ( int i=0; i<facesQty; i++ )
    {
        const Face & f = faces[i];
        if ( !f.leaf )
            continue;
        for ( int j=0; j<4; j++ )
        {
            const int vertInd = f.vertexInds[j];
            Vertex & v = verts[vertInd];
            v.leafFacesQty += 1;
        }
    }

    // It is modifiable if it belongs to
    // at least 3 leaf faces
    for ( int i=0; i<qty; i++ )
    {
        Vertex & v   = verts[i];
        const bool enoughFaces = (v.leafFacesQty > 2);
        v.isMidPoint = !enoughFaces;
    }

}

void Cubesphere::scaleToSphere()
{
    const int qty = (int)verts.Size();
    for ( int i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        if ( !v.isMidPoint )
        {
            v.at.Normalize();
            v.norm = v.at;
        }
    }

    for ( int i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        if ( v.isMidPoint )
        {
            const int vertIndA = v.a;
            const int vertIndB = v.b;
            const Vertex & va = verts[vertIndA];
            const Vertex & vb = verts[vertIndB];
            v.at = (va.at + vb.at) * 0.5;
            v.norm = va.norm + vb.norm;
            v.norm.Normalize();
        }
    }
}

void Cubesphere::computeNormals()
{
    // First zero all normals.
    const int qty = (int)verts.Size();
    for ( int i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        v.norm = Vector3d::ZERO;
    }

    const int facesQty = (int)faces.Size();
    // Concatenate all normals.
    for ( int i=0; i<facesQty; i++ )
    {
        const Face & f = faces[i];
        if ( !f.leaf )
            continue;
        {
            const int vInd0 = f.vertexInds[0];
            const int vInd1 = f.vertexInds[1];
            const int vInd2 = f.vertexInds[2];
            const int vInd3 = f.vertexInds[3];
            Vertex & v0 = verts[vInd0];
            Vertex & v1 = verts[vInd1];
            Vertex & v2 = verts[vInd2];
            Vertex & v3 = verts[vInd3];
            const Vector3d a = v1.at - v0.at;
            const Vector3d b = v2.at - v0.at;
            Vector3d n = a.CrossProduct( b );
            n.Normalize();
            v0.norm += n;
            v1.norm += n;
            v2.norm += n;
        }
    }

    // Normalize normals.
    for ( int i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        v.norm.Normalize();
    }
}

void Cubesphere::applySource( Source * src, Vertex & v )
{
    const Float dh = src->dh( v.at );
    const Float d  = 1.0 + dh;
    v.at = v.at * d;
}


}



