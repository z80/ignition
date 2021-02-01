
#include "subdivide_source_ref.h"


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

// Let's for now only use ref_frame position relative to cubesphere ref frame.
// Of course, it is correct to take every object inside ref frame and compute that way.
bool SubdivideSourceRef::need_subdivide( Node * ref_frame, Node * cubesphere_node )
{
    return false;
}






}










