
#include "cube_sphere.h"

namespace Ign
{

SubdriveSource::SubdriveSource()
{
    r_ = 1.0;

    done_ = false;
}

SubdriveSource::~SubdriveSource()
{

}

void SubdriveSource::DrawDebugGeometry( float scale, DebugRenderer * debug, bool depthTest ) const
{
    const Color C( Color::BLUE );
    const Color CF( Color::MAGENTA );
    const unsigned qty = pts_.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        const Vector3d & pt = pts_[i];
        const Vector3d & ptFlat = ptsFlat_[i];
        const Vector3 ptf( pt.x_, pt.y_, pt.z_ );
        const Vector3 ptFlatf( ptFlat.x_, ptFlat.y_, ptFlat.z_ );
        debug->AddCross( ptf*scale, 1.0, C, depthTest );
        debug->AddCross( ptFlatf*scale, 1.0, CF, depthTest );
        debug->AddLine( ptf*scale, ptFlatf*scale, C, depthTest );
    }
}

void SubdriveSource::setR( Float r )
{
    r_ = r;
}

void SubdriveSource::clearLevels()
{
    levels_.Clear();
}

void SubdriveSource::addLevel( Float sz, Float dist )
{
    Level lvl;
    lvl.sz   = sz / r_;
    lvl.dist = dist / r_;
    levels_.Push( lvl );

    // Sort levels in distance accending order.
    const unsigned qty = levels_.Size();
    for ( unsigned i=0; i<(qty-1); i++ )
    {
        Level & a = levels_[i];
        for ( unsigned j=(i+1); j<qty; j++ )
        {
            Level & b = levels_[j];
            if ( a.dist > b.dist )
            {
                const Level c = a;
                a = b;
                b = c;
            }
        }
    }
}

bool SubdriveSource::needSubdrive( const Cubesphere * s, Vector<Vector3d> & pts )
{
    ptsNew_ = pts;
    const unsigned ptsNewQty = ptsNew_.Size();
    for ( unsigned i=0; i<ptsNewQty; i++ )
    {
        Vector3d & v = ptsNew_[i];
        v.Normalize();
    }

    if ( levels_.Empty() )
    {
        pts_ = ptsNew_;
        flattenPts( s );
        return true;
    }

    const Level lvl = levels_[0];
    const Float d = lvl.dist / r_ * 0.5;


    // Check all distances.
    const unsigned ptsQty = pts_.Size();
    bool needSubdrive = false;
    if ( ( ptsQty < 1 ) || ( ptsNewQty > ptsQty ) )
        needSubdrive = true;
    else
    {
        // Check distances.
        for ( unsigned i=0; i<ptsNewQty; i++ )
        {
            const Vector3d & v = ptsNew_[i];
            Float minDist = -1.0;
            for ( unsigned j=0; j<ptsQty; j++ )
            {
                const Vector3d & a = pts_[j];
                const Float dot = v.DotProduct( a );
                const Vector3d proj = a*dot;
                const Vector3d diff = v - proj;
                const Float dist = diff.Length();
                if ( (minDist < 0.0) || (dist < minDist) )
                    minDist = dist;
            }
            if ( minDist >= d )
            {
                needSubdrive = true;
                break;
            }
        }
    }

    if ( needSubdrive )
    {
        pts_ = ptsNew_;
        flattenPts( s );
    }

    return needSubdrive;
}

bool SubdriveSource::needSubdrive( const Cubesphere * s, const Face * f ) const
{
    const Float sz = f->size( s );
    const Vector3d n = f->normal( s );
    const unsigned ptsQty = ptsFlat_.Size();
    const Float levelsQty = levels_.Size();
    for ( unsigned i=0; i<ptsQty; i++ )
    {
        const Vector3d & a = ptsFlat_[i];
        //const bool ok = f->correctSide( n, a );
        //if ( !ok )
        //    continue;
        for ( unsigned j=0; j<levelsQty; j++ )
        {
            const Level & lvl = levels_[j];
            const bool inside = f->inside( s, a, n, lvl.dist );
            if ( inside )
            {
                if ( sz > lvl.sz )
                    return true;
            }
        }
    }
    return false;

    /*if ( done_ )
        return false;

    if ( ( f->parentInd < 0 ) && ( f->indexInParent == 0 ) )
    {
        done_ = true;
        return true;
    }

    return false;*/
}

void SubdriveSource::flattenPts( const Cubesphere * s )
{
    s->flattenPts( pts_, ptsFlat_ );
}

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
    this->atFlat = atFlat;
    this->at  = at;
}

Vertex::Vertex( const Vertex & inst )
{
    *this = inst;
}

const Vertex & Vertex::operator=( const Vertex & inst )
{
    if ( this != &inst )
    {
        atFlat  = inst.atFlat;
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

void Face::DrawDebugGeometry( float scale, const Cubesphere * s, DebugRenderer * debug, bool depthTest, bool flat ) const
{
    if ( !flat )
    {
        if ( !leaf )
            return;
    }

    const Vertex & va = s->verts[vertexInds[0]];
    const Vertex & vb = s->verts[vertexInds[1]];
    const Vertex & vc = s->verts[vertexInds[2]];
    const Vertex & vd = s->verts[vertexInds[3]];

    const Color C = flat ? Color::GREEN : Color::MAGENTA;

    if ( flat )
    {
        {
            const Vector3 a( va.atFlat.x_, va.atFlat.y_, va.atFlat.z_ );
            const Vector3 b( vb.atFlat.x_, vb.atFlat.y_, vb.atFlat.z_ );
            debug->AddLine( a*scale, b*scale, C, depthTest );
        }
        {
            const Vector3 a( vb.atFlat.x_, vb.atFlat.y_, vb.atFlat.z_ );
            const Vector3 b( vc.atFlat.x_, vc.atFlat.y_, vc.atFlat.z_ );
            debug->AddLine( a*scale, b*scale, C, depthTest );
        }
        {
            const Vector3 a( vc.atFlat.x_, vc.atFlat.y_, vc.atFlat.z_ );
            const Vector3 b( vd.atFlat.x_, vd.atFlat.y_, vd.atFlat.z_ );
            debug->AddLine( a*scale, b*scale, C, depthTest );
        }
        {
            const Vector3 a( vd.atFlat.x_, vd.atFlat.y_, vd.atFlat.z_ );
            const Vector3 b( va.atFlat.x_, va.atFlat.y_, va.atFlat.z_ );
            debug->AddLine( a*scale, b*scale, C, depthTest );
        }
    }
    else
    {
        {
            const Vector3 a( va.at.x_, va.at.y_, va.at.z_ );
            const Vector3 b( vb.at.x_, vb.at.y_, vb.at.z_ );
            debug->AddLine( a*scale, b*scale, C, depthTest );
        }
        {
            const Vector3 a( vb.at.x_, vb.at.y_, vb.at.z_ );
            const Vector3 b( vc.at.x_, vc.at.y_, vc.at.z_ );
            debug->AddLine( a*scale, b*scale, C, depthTest );
        }
        {
            const Vector3 a( vc.at.x_, vc.at.y_, vc.at.z_ );
            const Vector3 b( vd.at.x_, vd.at.y_, vd.at.z_ );
            debug->AddLine( a*scale, b*scale, C, depthTest );
        }
        {
            const Vector3 a( vd.at.x_, vd.at.y_, vd.at.z_ );
            const Vector3 b( va.at.x_, va.at.y_, va.at.z_ );
            debug->AddLine( a*scale, b*scale, C, depthTest );
        }
    }
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

bool Face::subdrive( Cubesphere * s, SubdriveSource * src )
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

const Vector3d Face::normal( const Cubesphere * s ) const
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

const Float Face::size( const Cubesphere * s ) const
{
    const Vertex & v0 = s->verts[ vertexInds[0] ];
    const Vertex & v1 = s->verts[ vertexInds[1] ];
    const Vector3d d = v1.atFlat - v0.atFlat;
    const Float sz = d.Length();
    return sz;
}

bool Face::inside( const Cubesphere * s, const Vector3d & a, const Vector3d & n, const Float dist ) const
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

bool Face::selectLeafs( const Cubesphere * s, const Vector3d & a, const Float dist, Vector<int> & faceInds ) const
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
        const Face & f = s->faces[ childInds[i] ];
        const bool insideOk = f.selectLeafs( s, a, dist, faceInds );
        if ( insideOk )
            faceInds.Push( childInd );
    }
    return false;
}

bool Face::correctSide( const Vector3d & n, const Vector3d & a )
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

bool Face::centralProjection( const Vector3d & n, const Vector3d & a, Vector3d & proj )
{
    // Anything smaller than 0.5 should work. Right?
    const Float EPS = 0.3;
    // Project on face plane.
    // But not perpendicular projection.
    // It is a projection along "a" in such a way that makes (a, n) = 1.
    const Float a_n = a.DotProduct( n );
    if ( a_n < EPS )
        return false;
    
    proj = a / a_n;
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

unsigned EdgeHash::ToHash() const
{
    long long a = 0;
    for ( int i=0; i<EDGE_HASH_SZ; i++ )
        a = (a << 8) + d[i];
    const unsigned h = MakeHash( a );
    return h;
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

    // Initial 8 vertices and 6 faces.
    init();
}

Cubesphere::~Cubesphere()
{

}

void Cubesphere::DrawDebugGeometry( DebugRenderer * debug, bool depthTest, bool flat ) const
{
    const float SCALE = 30.0;
    const unsigned faceQty = faces.Size();
    for ( unsigned i=0; i<faceQty; i++ )
    {
        const Face & f = faces[i];
        f.DrawDebugGeometry( SCALE, this, debug, depthTest, flat );
    }
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

void Cubesphere::subdrive( SubdriveSource * src )
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
            return;
    }

    //labelMidPoints();
    //scaleToSphere();
    //// Here might not be needed.
    ////computeNormals();
    //applySource( src );
}

void Cubesphere::applySource( HeightSource * src )
{
    labelMidPoints();
    scaleToSphere();

    if ( !src )
        return;

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

void Cubesphere::triangleList( const Vector<Vector3d> & pts, Float dist, Vector<Vector3d> & tris )
{
    selectFaces( pts, dist, faceInds_ );

    const unsigned qty = faceInds_.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        const unsigned ind = faceInds_[i];
        const Face & f = faces[ind];
        if ( f.leaf )
        {
            const int ind0 = f.vertexInds[0];
            const int ind1 = f.vertexInds[1];
            const int ind2 = f.vertexInds[2];
            const int ind3 = f.vertexInds[3];
            tris.Push( this->verts[ind0].at );
            tris.Push( this->verts[ind1].at );
            tris.Push( this->verts[ind2].at );
            tris.Push( this->verts[ind0].at );
            tris.Push( this->verts[ind2].at );
            tris.Push( this->verts[ind3].at );
        }
    }
}

void Cubesphere::flattenPts( const Vector<Vector3d> & pts, Vector<Vector3d> & ptsFlat ) const
{
    // Use first 6 faces in the cubesphere to project
    // all the points onto appropriate faces.
    Vector3d norms[6];
    for ( unsigned i=0; i<6; i++ )
    {
        const Face & face = this->faces[i];
        norms[i] = face.normal( this );
    }

    ptsFlat = pts;
    const unsigned ptsQty = ptsFlat.Size();
    for ( unsigned i=0; i<ptsQty; i++ )
    {
        Vector3d & pt3 = ptsFlat[i];
        for ( int j=0; j<6; j++ )
        {
            const Vector3d & n = norms[j];
            Vector3d proj;
            const bool ok = Face::centralProjection( n, pt3, proj );
            if ( ok )
            {
                pt3 = proj;
                break;
            }
        }
    }
}

void Cubesphere::clear()
{
    verts.Clear();
    faces.Clear();
    lookup.Clear();
}

void Cubesphere::init()
{
    Vertex v;
    v.atFlat = Vector3d( -1.0, -1.0, -1.0 );
    verts.Push( v );

    v.atFlat = Vector3d( -1.0, -1.0,  1.0 );
    verts.Push( v );

    v.atFlat = Vector3d(  1.0, -1.0,  1.0 );
    verts.Push( v );

    v.atFlat = Vector3d(  1.0, -1.0, -1.0 );
    verts.Push( v );

    v.atFlat = Vector3d( -1.0,  1.0, -1.0 );
    verts.Push( v );

    v.atFlat = Vector3d( -1.0,  1.0,  1.0 );
    verts.Push( v );

    v.atFlat = Vector3d(  1.0,  1.0,  1.0 );
    verts.Push( v );

    v.atFlat = Vector3d(  1.0,  1.0, -1.0 );
    verts.Push( v );

    Face f;
    f.leaf  = true;
    f.level = 0;
    f.parentInd = -1;
    f.indexInParent = 0;
    f.childInds[0] = -1;
    f.childInds[1] = -1;
    f.childInds[2] = -1;
    f.childInds[3] = -1;

    f.vertexInds[0] = 0;
    f.vertexInds[1] = 1;
    f.vertexInds[2] = 2;
    f.vertexInds[3] = 3;
    faces.Push( f );

    f.indexInParent = 1;
    f.vertexInds[0] = 4;
    f.vertexInds[1] = 5;
    f.vertexInds[2] = 1;
    f.vertexInds[3] = 0;
    faces.Push( f );

    f.indexInParent = 2;
    f.vertexInds[0] = 7;
    f.vertexInds[1] = 6;
    f.vertexInds[2] = 5;
    f.vertexInds[3] = 4;
    faces.Push( f );

    f.indexInParent = 3;
    f.vertexInds[0] = 3;
    f.vertexInds[1] = 2;
    f.vertexInds[2] = 6;
    f.vertexInds[3] = 7;
    faces.Push( f );

    f.indexInParent = 4;
    f.vertexInds[0] = 1;
    f.vertexInds[1] = 5;
    f.vertexInds[2] = 6;
    f.vertexInds[3] = 2;
    faces.Push( f );

    f.indexInParent = 5;
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
            v.at = v.atFlat;
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

void Cubesphere::applySource( HeightSource * src, Vertex & v )
{
    const Float dh = src->height( v.at );
    const Float d  = 1.0 + dh;
    v.at = v.at * d;
}

void Cubesphere::selectFaces( const Vector<Vector3d> & pts, const Float dist, Vector<int> & faceInds )
{
    faceInds.Clear();
    flattenPts( pts, ptsFlat_ );

    const unsigned ptsQty = ptsFlat_.Size();
    for ( unsigned ptInd=0; ptInd<ptsQty; ptInd++ )
    {
        const Vector3d & ptFlat = ptsFlat_[ptInd];
        for ( unsigned i=0; i<6; i++ )
        {
            const Face & f = faces[i];
            const Vector3d n = f.normal( this );
            const bool inside = f.inside( this, ptFlat, n, dist );
            if ( !inside )
                continue;
            f.selectLeafs( this, ptFlat, dist, faceInds );
        }
    }
}

void CubeSphereComponent::RegisterComponent( Context * context )
{
    context->RegisterFactory<CubeSphereComponent>();
}

CubeSphereComponent::CubeSphereComponent( Context * context )
    : Component( context )
{

}

CubeSphereComponent::~CubeSphereComponent()
{

}

void CubeSphereComponent::DrawDebugGeometry( DebugRenderer * debug, bool depthTest )
{
    subdriveSource_.DrawDebugGeometry( 30.0, debug, depthTest );
    cubesphereCube_.DrawDebugGeometry( debug, depthTest );
    cubesphereSphere_.DrawDebugGeometry( debug, depthTest, false );

    {
        const Color C( Color::YELLOW );

        unsigned qty = tris_.Size();
        if ( qty < 1 )
            return;
        for ( unsigned i=0; i<qty; i+=3 )
        {
            const Vector3d & va = tris_[i];
            const Vector3d & vb = tris_[i+1];
            const Vector3d & vc = tris_[i+2];
            const Vector3 a( va.x_, va.y_, va.z_ );
            const Vector3 b( vb.x_, vb.y_, vb.z_ );
            const Vector3 c( vc.x_, vc.y_, vc.z_ );
            debug->AddLine( a*25.0, b*25.0, C, depthTest );
            debug->AddLine( b*25.0, c*25.0, C, depthTest );
            debug->AddLine( c*25.0, a*25.0, C, depthTest );
        }
    }
}



}



