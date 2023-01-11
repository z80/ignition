
#include "bounding_node_gd.h"


namespace Ign
{

void BoundingNodeGd::_bind_methods()
{
}

BoundingNodeGd::BoundingNodeGd()
	: Reference()
{
	ClassDB::bind_method( D_METHOD("create_adjacent_node", "dx", "dy", "dz"), &BoundingNodeGd::create_adjacent_node, Variant::OBJECT );
}

BoundingNodeGd::~BoundingNodeGd()
{
}

Ref<BoundingNodeGd> BoundingNodeGd::create_adjacent_node( int dx, int dy, int dz ) const
{
	Ref<BoundingNodeGd> ret;
	ret.instance();
	ret->node = node.create_adjacent_node( dx, dy, dz );

	return ret;
}

}

