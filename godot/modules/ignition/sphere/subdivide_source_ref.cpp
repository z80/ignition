
#include "subdivide_source_ref.h"
#include "ref_frame.h"


namespace Ign
{

void SubdivideSourceRef::_bind_methods()
{
}

SubdivideSourceRef::SubdivideSourceRef()
    : Reference()
{
}

SubdivideSourceRef::~SubdivideSourceRef()
{
}

void SubdivideSourceRef::clear_levels()
{
    subdivide_source.clear_levels();
}

void SubdivideSourceRef::add_level( Float sz, Float dist )
{
    subdivide_source.add_level( sz, dist );
}

bool SubdivideSourceRef::need_subdivide( Node * ref_frame, Node * cubesphere )
{
    CubeSphereNode * csn = Object::cast_to<CubeSphereNode>( cubesphere );
    if ( csn == nullptr )
        return false;

    RefFrameNode * rf = Object::cast_to<RefFrameNode>(ref_frame);
    if (rf == nullptr)
    {
        Spatial * s = Object::cast_to<Spatial>( ref_frame );
        SubdividePoint sp;
        const Vector3 at = s->get_transform().origin;
        sp.at = Vector3d( at.x, at.y, at.z );
        sp.close = true;

        points.clear();
        points.push_back( sp );
        const bool ret = subdivide_source.need_subdivide( csn, points );
        return ret;
    }

    const SE3 se3 = rf->relative_( csn ); 
    SubdividePoint sp;
    sp.at    = se3.r_;
    sp.close = true;

    points.clear();
    points.push_back( sp );

    const bool ret = subdivide_source.need_subdivide( csn, points );
     
    return ret;
}






}










