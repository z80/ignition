
#include "cube_quad_node.h"

namespace Ign
{

CubeQuadNode::CubeQuadNode()
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

CubeQuadNode::~CubeQuadNode()
{
}


CubeQuadNode::CubeQuadNode( int a, int b, int c, int d )
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

CubeQuadNode::CubeQuadNode( const CubeQuadNode & inst )
{
    *this = inst;
}

const CubeQuadNode & CubeQuadNode::operator=( const CubeQuadNode & inst )
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

bool CubeQuadNode::subdivide( CubeSphere * s, SubdriveSource * src )
{
    if ( !leaf )
    {
        for ( int i=0; i<4; i++ )
        {
            const int faceInd = childInds[i];
            CubeQuadNode f = s->faces[faceInd];
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
        HashMap<EdgeHash, int>::ConstIterator it = s->lookup.Find( hashN );
        if ( it == s->lookup.End() )
        {
            const Vertex & va = s->verts[vertIndA];
            const Vertex & vb = s->verts[vertIndB];
            // Create new vertex;
            const Vector3d atFlat( (va.atFlat + vb.atFlat) * 0.5 );
            Vertex v;
            v.atFlat = atFlat;
            v.a = vertIndA;
            v.b = vertIndB;
            const int newInd = (int)s->verts.Size();
            s->verts.Push( v );
            s->lookup.Insert( Pair<EdgeHash, int>( hashN, newInd ) );
            newVertInds[i] = newInd;
        }
        else
        {
            const int vertInd = it->second_;
            //Vertex & v = s->verts[vertInd];
            newVertInds[i] = vertInd;
        }
    }
    // Also create middle vertex.
    const Vertex & a = s->verts[newVertInds[0]];
    const Vertex & b = s->verts[newVertInds[1]];
    const Vertex & c = s->verts[newVertInds[2]];
    const Vertex & d = s->verts[newVertInds[3]];

    const Vector3d atFlat( (a.atFlat + b.atFlat + c.atFlat + d.atFlat)*0.25 );
    Vertex v;
    v.atFlat = atFlat;
    v.a = -1;
    v.b = -1;
    const int fifthVertInd = (int)s->verts.Size();
    s->verts.Push( v );
    newVertInds[4] = fifthVertInd;

    // Create faces.
    CubeQuadNode fa( vertexInds[0],  newVertInds[0], newVertInds[4], newVertInds[3] );
    CubeQuadNode fb( newVertInds[0],  vertexInds[1], newVertInds[1], newVertInds[4] );
    CubeQuadNode fc( newVertInds[4], newVertInds[1],  vertexInds[2], newVertInds[2] );
    CubeQuadNode fd( newVertInds[3], newVertInds[4], newVertInds[2],  vertexInds[3] );
    fa.level = newLevel;
    fa.leaf  = true;
    fa.indexInParent = 0;
    fa.hash_ = hash_;
    fa.hash_ << fa.indexInParent;

    fb.level = newLevel;
    fb.leaf  = true;
    fb.indexInParent = 1;
    fb.hash_ = hash_;
    fb.hash_ << fb.indexInParent;

    fc.level = newLevel;
    fc.leaf  = true;
    fc.indexInParent = 2;
    fc.hash_ = hash_;
    fc.hash_ << fc.indexInParent;

    fd.level = newLevel;
    fd.leaf  = true;
    fd.indexInParent = 3;
    fd.hash_ = hash_;
    fd.hash_ << fd.indexInParent;

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
        CubeQuadNode face = s->faces[faceInd];
        const bool ok = face.subdrive( s, src );
        s->faces[faceInd] = face;
        if ( !ok )
            return false;
    }

    return true;
}

const Vector3d CubeQuadNode::normal( const CubeSphere * s ) const
{
    const Vertex & v0 = s->verts[ vertexInds[0] ];
    const Vertex & v1 = s->verts[ vertexInds[1] ];
    const Vertex & v3 = s->verts[ vertexInds[3] ];
    const Vector3d a30 = v3.atFlat - v0.atFlat;
    const Vector3d a10 = v1.atFlat - v0.atFlat;
    Vector3d n = a30.CrossProduct( a10 );
    n.Normalize();
    return n;
}

const Float CubeQuadNode::size( const CubeSphere * s ) const
{
    const Vertex & v0 = s->verts[ vertexInds[0] ];
    const Vertex & v1 = s->verts[ vertexInds[1] ];
    const Vector3d d = v1.atFlat - v0.atFlat;
    const Float sz = d.Length();
    return sz;
}

bool CubeQuadNode::inside( const CubeSphere * s, const Vector3d & a, const Vector3d & n, const Float dist ) const
{
    const Vertex & v0 = s->verts[ vertexInds[0] ];
    const Vertex & v1 = s->verts[ vertexInds[1] ];
    //const Vertex & v2 = s->verts[ vertexInds[2] ];
    const Vertex & v3 = s->verts[ vertexInds[3] ];
    //const Vector3d at = (v0.atFlat + v1.atFlat + v2.atFlat + v3.atFlat) * 0.25;
    const Vector3d a30 = v3.atFlat - v0.atFlat;
    const Vector3d a10 = v1.atFlat - v0.atFlat;
    //Vector3d n = a30.CrossProduct( a10 );
    //n.Normalize();
    const Vector3d d = a - v0.atFlat;
    const Float ptToPlaneDist = std::abs( n.DotProduct( d ) );
    const Float inPlaneDist = dist - ptToPlaneDist;
    if ( inPlaneDist < 0.0 )
        return false;
    const Float a10Len = a10.Length();
    const Float projX = d.DotProduct( a10 ) / a10Len;
    if ( ( projX < -inPlaneDist ) || (projX > (a10Len+inPlaneDist)) )
        return false;
    const Float a30Len = a30.Length();
    const Float projY = d.DotProduct( a30 ) / a30Len;
    if ( ( projY < -inPlaneDist ) || (projY > (a30Len+inPlaneDist)) )
        return false;

    return true;
}

bool CubeQuadNode::select_leafs( const CubeSphere * s, const Vector3d & a, const Float dist, Vector<int> & faceInds ) const
{
    if ( leaf )
    {
        const Vector3d n = normal( s );
        const bool insideOk = inside( s, a, n, dist );
        return insideOk;
    }

    for ( int i=0; i<4; i++ )
    {
        const int childInd = childInds[i];
        if ( childInd < 0 )
            continue;
        const CubeQuadNode & f = s->faces[ childInds[i] ];
        const bool insideOk = f.selectLeafs( s, a, dist, faceInds );
        if ( insideOk )
            faceInds.Push( childInd );
    }
    return false;
}

bool CubeQuadNode::select_by_size( const CubeSphere * s, const Vector3d & a, const Float sz, const Float dist, Vector<int> & faceInds ) const
{
    const Float thisSize = size( s );
    const Vector3d n = normal( s );
    const bool insideOk = inside( s, a, n, dist );
    if ( (thisSize <= sz) && (insideOk) )
        return true;

    for ( int i=0; i<4; i++ )
    {
        const int childInd = childInds[i];
        if ( childInd < 0 )
            continue;
        const CubeQuadNode & f = s->faces[ childInds[i] ];
        const bool insideOk = f.selectBySize( s, a, sz, dist, faceInds );
        if ( insideOk )
            faceInds.Push( childInd );
    }
    return false;
}

bool CubeQuadNode::correct_side( const Vector3d & n, const Vector3d & a )
{
    const Float projLen = n.DotProduct( a );
    if ( projLen <= 0.0 )
        return false;
    const Vector3d proj = n * projLen;
    const Vector3d norm = a - proj;
    const Float normLen = norm.Length();
    const bool ok = ( projLen >= normLen );
    
    return ok;
}

bool CubeQuadNode::central_projection( const Vector3d & n, const Vector3d & a, Vector3d & proj )
{
    // Angle should be smaller than 45 deg.
	// It means that cosing shold be bigger than 1/sqrt(2).
	// And this number is approximately 0.707.
    const Float EPS = 0.707;
    // Project on face plane.
    // But not perpendicular projection.
    // It is a projection along "a" in such a way that makes (a, n) = 1.
    const Float a_n = a.DotProduct( n );
	if ( a_n >= EPS )
	{
		proj = a / a_n;
		return true;
	}
    
    return false;
}







}







