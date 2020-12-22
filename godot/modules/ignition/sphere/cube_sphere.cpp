
#include "cube_sphere.h"


namespace Ign
{



CubeSphere::CubeSphere()
{
    R_ = 10.0;
    H_ = 1.0;
    hash_ = 0;

    verts.Reserve( 4096 );
    faces.Reserve( 4096 );

    // Initial 8 vertices and 6 faces.
    init();
}

CubeSphere::~CubeSphere()
{

}



CubeSphere::CubeSphere( const CubeSphere & inst )
{
    *this = inst;
}

const CubeSphere & CubeSphere::operator=( const CubeSphere & inst )
{
    if ( this != &inst )
    {
        verts  = inst.verts;
        faces  = inst.faces;
        lookup = inst.lookup;

        R_        = inst.R_;
        H_        = inst.H_;
        ptsFlat_  = inst.ptsFlat_;
        faceInds_ = inst.faceInds_;
    }

    return *this;
}

void CubeSphere::set_hash( uint64_t hash )
{
    hash_ = hash;
}

void CubeSphere::set_r( const Float newR )
{
    R_ = newR;
}

Float CubeSphere::r() const
{
    return R_;
}

void CubeSphere::set_h( const Float newH )
{
    H_ = newH;
}

Float CubeSphere::h() const
{
    return H_;
}

void CubeSphere::subdrive( SubdivideSource * src )
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

void CubeSphere::apply_source( HeightSource * src )
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
            applySourceHeight( src, v );
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


    // Apply color information.
    for ( int i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        applySourceColor( src, v );
    }
}

void CubeSphere::triangle_list( Vector<Vertex> & tris )
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
            tris.Push( this->verts[ind2] );
            tris.Push( this->verts[ind1] );
            tris.Push( this->verts[ind0] );
            tris.Push( this->verts[ind3] );
            tris.Push( this->verts[ind2] );
        }
    }
}

void CubeSphere::triangle_list( const Vector<SubdivideSource::SubdividePoint> & pts, Float dist, Vector<Vertex> & tris )
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
            tris.Push( this->verts[ind0] );
            tris.Push( this->verts[ind2] );
            tris.Push( this->verts[ind1] );
            tris.Push( this->verts[ind0] );
            tris.Push( this->verts[ind3] );
            tris.Push( this->verts[ind2] );
        }
    }
}

void CubeSphere::face_list( const Vector<SubdivideSource::SubdividePoint> & pts, const Float sz, const Float dist, Vector<int> & faceInds )
{
    faceInds.Clear();
    flattenPts( pts, ptsFlat_ );

    const unsigned ptsQty = ptsFlat_.Size();
    for ( unsigned ptInd=0; ptInd<ptsQty; ptInd++ )
    {
        const Vector3d & ptFlat = ptsFlat_[ptInd].at;
        for ( unsigned i=0; i<6; i++ )
        {
            const Face & f = faces[i];
            const Vector3d n = f.normal( this );
            const bool inside = f.inside( this, ptFlat, n, dist );
            if ( !inside )
                continue;
            f.selectBySize( this, ptFlat, sz, dist, faceInds );
        }
    }
}

void CubeSphere::flatten_pts( const Vector<SubdivideSource::SubdividePoint> & pts, Vector<SubdivideSource::SubdividePoint> & ptsFlat ) const
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
        Vector3d & pt3 = ptsFlat[i].at;
        // Next 3 lines protect from zero length points.
        const Float pt3Len = pt3.Length();
        if ( pt3Len < 0.1 )
            pt3.x_ = 1.0;
        // Point must be normalized.
        pt3.Normalize();

        /*for ( int j=0; j<6; j++ )
        {
            const Vector3d & n = norms[j];
            Vector3d proj;
            const bool ok = Face::centralProjection( n, pt3, proj );
            if ( ok )
            {
                pt3 = proj;
                break;
            }
        }*/
        const Float absX = std::abs( pt3.x_ );
        const Float absY = std::abs( pt3.y_ );
        const Float absZ = std::abs( pt3.z_ );
        if ( ( absX >= absY ) && ( absX >= absZ ) )
            pt3 = pt3 / absX;
        else if ( ( absY >= absX ) && ( absY >= absZ ) )
            pt3 = pt3 / absY;
        else
            pt3 = pt3 / absZ;
    }
}

void CubeSphere::clear()
{
    verts.Clear();
    faces.Clear();
    lookup.Clear();
}

void CubeSphere::init()
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
    f.hash_.reset( hash_ );
    f.hash_ << f.indexInParent;
    faces.Push( f );

    f.indexInParent = 1;
    f.vertexInds[0] = 4;
    f.vertexInds[1] = 5;
    f.vertexInds[2] = 1;
    f.vertexInds[3] = 0;
    f.hash_.reset( hash_ );
    f.hash_ << f.indexInParent;
    faces.Push( f );

    f.indexInParent = 2;
    f.vertexInds[0] = 7;
    f.vertexInds[1] = 6;
    f.vertexInds[2] = 5;
    f.vertexInds[3] = 4;
    f.hash_.reset( hash_ );
    f.hash_ << f.indexInParent;
    faces.Push( f );

    f.indexInParent = 3;
    f.vertexInds[0] = 3;
    f.vertexInds[1] = 2;
    f.vertexInds[2] = 6;
    f.vertexInds[3] = 7;
    f.hash_.reset( hash_ );
    f.hash_ << f.indexInParent;
    faces.Push( f );

    f.indexInParent = 4;
    f.vertexInds[0] = 1;
    f.vertexInds[1] = 5;
    f.vertexInds[2] = 6;
    f.vertexInds[3] = 2;
    f.hash_.reset( hash_ );
    f.hash_ << f.indexInParent;
    faces.Push( f );

    f.indexInParent = 5;
    f.vertexInds[0] = 4;
    f.vertexInds[1] = 0;
    f.vertexInds[2] = 3;
    f.vertexInds[3] = 7;
    f.hash_.reset( hash_ );
    f.hash_ << f.indexInParent;
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

void CubeSphere::label_mid_points()
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

void CubeSphere::scale_to_sphere()
{
    const int qty = (int)verts.Size();
    for ( int i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        if ( !v.isMidPoint )
        {
            v.atUnit = v.atFlat;
            v.atUnit.Normalize();
            v.norm = v.atUnit;
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
            v.atUnit = (va.atUnit + vb.atUnit) * 0.5;
            v.norm = va.norm + vb.norm;
            v.norm.Normalize();
        }
    }
}

void CubeSphere::compute_normals()
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

void CubeSphere::apply_source_height( HeightSource * src, Vertex & v )
{
    v.heightUnit_ = src->height( v.atUnit );
    const Float d  = R_ + H_*v.heightUnit_;
    v.at = v.atUnit * d;
}

void CubeSphere::apply_source_color( HeightSource * src, Vertex & v )
{
    const Color c = src->color( v.atUnit, v.norm, v.heightUnit_ );
    v.color = c;
}


void CubeSphere::select_faces( const Vector<SubdivideSource::SubdividePoint> & pts, const Float dist, Vector<int> & faceInds )
{
    faceInds.Clear();
    flattenPts( pts, ptsFlat_ );

    const unsigned ptsQty = ptsFlat_.Size();
    for ( unsigned ptInd=0; ptInd<ptsQty; ptInd++ )
    {
        const Vector3d & ptFlat = ptsFlat_[ptInd].at;
        for ( unsigned i=0; i<6; i++ )
        {
            const Face & f = faces[i];
            const Vector3d n = f.normal( this );
            const bool inside = f.inside( this, ptFlat, n, dist );
            if ( !inside )
                continue;
            const bool insideOk = f.selectLeafs( this, ptFlat, dist, faceInds );
            if ( insideOk )
                faceInds.Push( i );
        }
    }
}





}






