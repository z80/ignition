
#include "subdivide_source_ref.h"
#include "ref_frame_node.h"
#include "cube_sphere_node.h"


namespace Ign
{

void SubdivideSourceRef::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("need_subdivide", "ref_frame", "cubesphere_node"), &SubdivideSourceRef::need_subdivide, Variant::BOOL );
	ClassDB::bind_method( D_METHOD("force_subdivide"), &SubdivideSourceRef::force_subdivide );
}

SubdivideSourceRef::SubdivideSourceRef()
    : Reference()
{
	source = &subdivide_source;
}

SubdivideSourceRef::~SubdivideSourceRef()
{
}

// Let's for now only use ref_frame position relative to cubesphere ref frame.
// Of course, it is correct to take every object inside ref frame and compute that way.
bool SubdivideSourceRef::need_subdivide( Node * ref_frame, Node * cubesphere_node )
{
    CubeSphereNode * csn = Object::cast_to<CubeSphereNode>( cubesphere_node );
    if ( csn == nullptr )
        return false;

    RefFrameNode * rf = Object::cast_to<RefFrameNode>(ref_frame);
    if (rf == nullptr)
    {
        Spatial * s = Object::cast_to<Spatial>( ref_frame );
        const Vector3 at = s->get_transform().origin;
        const Vector3d sp = Vector3d( at.x, at.y, at.z );

        subdivide_points.clear();
        subdivide_points.push_back( sp );
        const bool ret = source->need_subdivide( &(csn->sphere), subdivide_points );
        return ret;
    }

    const SE3 se3 = rf->relative_( csn ); 
    subdivide_points.clear();
    subdivide_points.push_back( se3.r_ );

    const bool ret = source->need_subdivide( &(csn->sphere), subdivide_points );
     
    return ret;
}

void SubdivideSourceRef::force_subdivide()
{
	source->force_subdivide();
}






}










