
#include "cube_sphere.h"
#include "cube_quad_node.h"
#include "height_source.h"

namespace Ign
{



CubeSphere::CubeSphere()
{
    R_ = 10.0;
    H_ = 1.0;
    hash_ = 0;

    //verts.reserve( 4096 );
    //faces.reserve( 4096 );

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

void CubeSphere::subdivide( SubdivideSource * src )
{
    clear();
    init();

    const int qty = faces.size();
    for ( int i=0; i<qty; i++ )
    {
        CubeQuadNode f = faces.ptr()[i];
        if ( !f.leaf )
            continue;
        const bool ok = f.subdivide( this, src );
        faces.ptrw()[i] = f;
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
    label_mid_points();
    scale_to_sphere();

    //if ( !src )
    //    return;

    const int qty = verts.size();
    for ( int i=0; i<qty; i++ )
    {
        CubeVertex & v = verts.ptrw()[i];
        if ( !v.isMidPoint )
            apply_source_height( src, v );
    }

    for ( int i=0; i<qty; i++ )
    {
        CubeVertex & v = verts.ptrw()[i];
        if ( v.isMidPoint )
        {
            const int vertIndA = v.a;
            const int vertIndB = v.b;
            const CubeVertex & va = verts.ptr()[vertIndA];
            const CubeVertex & vb = verts.ptr()[vertIndB];
            v.at = (va.at + vb.at) * 0.5;
            v.norm = va.norm + vb.norm;
            v.norm.Normalize();
        }
    }
    compute_normals();


    // Apply color information.
    for ( int i=0; i<qty; i++ )
    {
        CubeVertex & v = verts.ptrw()[i];
        apply_source_color( src, v );
    }
}

void CubeSphere::triangle_list( Vector<CubeVertex> & tris )
{
    tris.clear();
    const unsigned qty = faces.size();
    for ( unsigned i=0; i<qty; i++ )
    {
        const CubeQuadNode & f = faces.ptr()[i];
        if ( f.leaf )
        {
            const int ind0 = f.vertexInds[0];
            const int ind1 = f.vertexInds[1];
            const int ind2 = f.vertexInds[2];
            const int ind3 = f.vertexInds[3];
            tris.push_back( this->verts[ind0] );
            tris.push_back( this->verts[ind1] );
            tris.push_back( this->verts[ind2] );
            tris.push_back( this->verts[ind0] );
            tris.push_back( this->verts[ind2] );
            tris.push_back( this->verts[ind3] );
        }
    }
}

void CubeSphere::triangle_list( const Vector<SubdivideSource::SubdividePoint> & pts, Float dist, Vector<CubeVertex> & tris )
{
    select_faces( pts, dist, faceInds_ );

    const unsigned qty = faceInds_.size();
    for ( unsigned i=0; i<qty; i++ )
    {
        const unsigned ind = faceInds_.ptr()[i];
        const CubeQuadNode & f = faces.ptr()[ind];
        if ( f.leaf )
        {
            const int ind0 = f.vertexInds[0];
            const int ind1 = f.vertexInds[1];
            const int ind2 = f.vertexInds[2];
            const int ind3 = f.vertexInds[3];
            tris.push_back( this->verts[ind0] );
            tris.push_back( this->verts[ind2] );
            tris.push_back( this->verts[ind1] );
            tris.push_back( this->verts[ind0] );
            tris.push_back( this->verts[ind3] );
            tris.push_back( this->verts[ind2] );
        }
    }
}

void CubeSphere::face_list( const Vector<SubdivideSource::SubdividePoint> & pts, const Float sz, const Float dist, Vector<int> & faceInds )
{
	const Float sz_unit = sz / (R_*PI2*0.25);
    faceInds.clear();
    flatten_pts( pts, ptsFlat_ );

    const unsigned ptsQty = ptsFlat_.size();
    for ( unsigned ptInd=0; ptInd<ptsQty; ptInd++ )
    {
        const Vector3d & ptFlat = ptsFlat_.ptr()[ptInd].at;
        for ( unsigned i=0; i<6; i++ )
        {
            const CubeQuadNode & f = faces.ptr()[i];
            const Vector3d n = f.normal( this );
            const bool inside = f.inside( this, ptFlat, n, dist );
            if ( !inside )
                continue;
            f.select_by_size( this, ptFlat, sz_unit, dist, faceInds );
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
        const CubeQuadNode & face = this->faces.ptr()[i];
        norms[i] = face.normal( this );
    }

    ptsFlat = pts;
    const unsigned ptsQty = ptsFlat.size();
    for ( unsigned i=0; i<ptsQty; i++ )
    {
        Vector3d & pt3 = ptsFlat.ptrw()[i].at;
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
    verts.clear();
    faces.clear();
    lookup.clear();
}

void CubeSphere::init()
{
    CubeVertex v;
    v.atFlat = Vector3d( -1.0, -1.0, -1.0 );
    verts.push_back( v );

    v.atFlat = Vector3d( -1.0, -1.0,  1.0 );
    verts.push_back( v );

    v.atFlat = Vector3d(  1.0, -1.0,  1.0 );
    verts.push_back( v );

    v.atFlat = Vector3d(  1.0, -1.0, -1.0 );
    verts.push_back( v );

    v.atFlat = Vector3d( -1.0,  1.0, -1.0 );
    verts.push_back( v );

    v.atFlat = Vector3d( -1.0,  1.0,  1.0 );
    verts.push_back( v );

    v.atFlat = Vector3d(  1.0,  1.0,  1.0 );
    verts.push_back( v );

    v.atFlat = Vector3d(  1.0,  1.0, -1.0 );
    verts.push_back( v );

    CubeQuadNode f;
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
    faces.push_back( f );

    f.indexInParent = 1;
    f.vertexInds[0] = 4;
    f.vertexInds[1] = 5;
    f.vertexInds[2] = 1;
    f.vertexInds[3] = 0;
    f.hash_.reset( hash_ );
    f.hash_ << f.indexInParent;
    faces.push_back( f );

    f.indexInParent = 2;
    f.vertexInds[0] = 7;
    f.vertexInds[1] = 6;
    f.vertexInds[2] = 5;
    f.vertexInds[3] = 4;
    f.hash_.reset( hash_ );
    f.hash_ << f.indexInParent;
    faces.push_back( f );

    f.indexInParent = 3;
    f.vertexInds[0] = 3;
    f.vertexInds[1] = 2;
    f.vertexInds[2] = 6;
    f.vertexInds[3] = 7;
    f.hash_.reset( hash_ );
    f.hash_ << f.indexInParent;
    faces.push_back( f );

    f.indexInParent = 4;
    f.vertexInds[0] = 1;
    f.vertexInds[1] = 5;
    f.vertexInds[2] = 6;
    f.vertexInds[3] = 2;
    f.hash_.reset( hash_ );
    f.hash_ << f.indexInParent;
    faces.push_back( f );

    f.indexInParent = 5;
    f.vertexInds[0] = 4;
    f.vertexInds[1] = 0;
    f.vertexInds[2] = 3;
    f.vertexInds[3] = 7;
    f.hash_.reset( hash_ );
    f.hash_ << f.indexInParent;
    faces.push_back( f );


    // Debugging.
    /*{
        const unsigned qty = verts.Size();
        for ( unsigned i=0; i<qty; i++ )
        {
            CubeVertex & v = verts[i];
            v.at = v.at * 0.001 + Vector3d( 0.0, 1.0, 0.0 );
        }
    }*/
}

void CubeSphere::label_mid_points()
{
    // Reset numebr of leaf faces it belongs to.
    const int qty = (int)verts.size();
    for ( int i=0; i<qty; i++ )
    {
        CubeVertex & v = verts.ptrw()[i];
        v.leafFacesQty = 0;
        v.isMidPoint   = false;
    }

    // Count number of leaf faces it belongs to.
    const int facesQty = faces.size();
    for ( int i=0; i<facesQty; i++ )
    {
        const CubeQuadNode & f = faces.ptr()[i];
        if ( !f.leaf )
            continue;
        for ( int j=0; j<4; j++ )
        {
            const int vertInd = f.vertexInds[j];
            CubeVertex & v = verts.ptrw()[vertInd];
            v.leafFacesQty += 1;
        }
    }

    // It is modifiable if it belongs to
    // at least 3 leaf faces
    for ( int i=0; i<qty; i++ )
    {
        CubeVertex & v   = verts.ptrw()[i];
        const bool enoughFaces = (v.leafFacesQty > 2);
        v.isMidPoint = !enoughFaces;
    }

}

void CubeSphere::scale_to_sphere()
{
    const int qty = (int)verts.size();
    for ( int i=0; i<qty; i++ )
    {
        CubeVertex & v = verts.ptrw()[i];
        if ( !v.isMidPoint )
        {
            v.atUnit = v.atFlat;
            v.atUnit.Normalize();
            v.norm = v.atUnit;
        }
    }

    for ( int i=0; i<qty; i++ )
    {
        CubeVertex & v = verts.ptrw()[i];
        if ( v.isMidPoint )
        {
            const int vertIndA = v.a;
            const int vertIndB = v.b;
            const CubeVertex & va = verts[vertIndA];
            const CubeVertex & vb = verts[vertIndB];
            v.atUnit = (va.atUnit + vb.atUnit) * 0.5;
            v.norm = va.norm + vb.norm;
            v.norm.Normalize();
        }
    }
}

void CubeSphere::compute_normals()
{
    // First zero all normals.
    const int qty = (int)verts.size();
    for ( int i=0; i<qty; i++ )
    {
        CubeVertex & v = verts.ptrw()[i];
        v.norm = Vector3d::ZERO;
    }

    const int facesQty = (int)faces.size();
    // Concatenate all normals.
    for ( int i=0; i<facesQty; i++ )
    {
        const CubeQuadNode & f = faces.ptr()[i];
        if ( !f.leaf )
            continue;
        {
            const int vInd0 = f.vertexInds[0];
            const int vInd1 = f.vertexInds[1];
            const int vInd2 = f.vertexInds[2];
            const int vInd3 = f.vertexInds[3];
            CubeVertex & v0 = verts.ptrw()[vInd0];
            CubeVertex & v1 = verts.ptrw()[vInd1];
            CubeVertex & v2 = verts.ptrw()[vInd2];
            CubeVertex & v3 = verts.ptrw()[vInd3];
            const Vector3d a = v1.at - v0.at;
            const Vector3d b = v2.at - v0.at;
            Vector3d n = a.CrossProduct( b );
            n.Normalize();
            v0.norm += n;
            v1.norm += n;
            v2.norm += n;

			const Vector3d c = v3.at - v0.at;
			n = c.CrossProduct( c );
			n.Normalize();
			v0.norm += n;
			v2.norm += n;
			v3.norm += n;
		}
    }

    // Normalize normals.
    for ( int i=0; i<qty; i++ )
    {
        CubeVertex & v = verts.ptrw()[i];
        v.norm.Normalize();
    }
}

void CubeSphere::apply_source_height( HeightSource * src, CubeVertex & v )
{
    v.heightUnit_ = (src != nullptr) ? src->height( v.atUnit ) : 0.0;
    const Float d  = R_ + H_*v.heightUnit_;
    v.at = v.atUnit * d;
}

void CubeSphere::apply_source_color( HeightSource * src, CubeVertex & v )
{
    const Color c = (src != nullptr) ? src->color( v.atUnit, v.norm, v.heightUnit_ ) : Color( 0.0, 0.7, 0.0, 1.0 );
    v.color = c;
}


void CubeSphere::select_faces( const Vector<SubdivideSource::SubdividePoint> & pts, const Float dist, Vector<int> & faceInds )
{
    faceInds.clear();
    flatten_pts( pts, ptsFlat_ );

    const unsigned ptsQty = ptsFlat_.size();
    for ( unsigned ptInd=0; ptInd<ptsQty; ptInd++ )
    {
        const Vector3d & ptFlat = ptsFlat_.ptr()[ptInd].at;
        for ( unsigned i=0; i<6; i++ )
        {
            const CubeQuadNode & f = faces.ptr()[i];
            const Vector3d n = f.normal( this );
            const bool inside = f.inside( this, ptFlat, n, dist );
            if ( !inside )
                continue;
            const bool insideOk = f.select_leafs( this, ptFlat, dist, faceInds );
            if ( insideOk )
                faceInds.push_back( i );
        }
    }
}





}






