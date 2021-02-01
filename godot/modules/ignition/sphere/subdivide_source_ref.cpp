
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

bool SubdivideSourceRef::need_subdivide( Node * ref_frame, Node * cubesphere )
{
    return false;
}






}










