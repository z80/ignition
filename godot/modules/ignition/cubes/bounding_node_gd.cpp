
#include "bounding_node_gd.h"


namespace Ign
{

void BoundingNodeGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("create_adjacent_node", "dx", "dy", "dz"), &BoundingNodeGd::create_adjacent_node, Variant::OBJECT );
	ClassDB::bind_method( D_METHOD("equals_to", "other_node"),                &BoundingNodeGd::equals_to,            Variant::BOOL );
	ClassDB::bind_method( D_METHOD("node_id"),                                &BoundingNodeGd::node_id,              Variant::STRING );
}

BoundingNodeGd::BoundingNodeGd()
	: Reference()
{
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

bool BoundingNodeGd::equals_to( const Ref<BoundingNodeGd> & other ) const
{
	const BoundingNodeGd * b = other.ptr();
	if (b == nullptr)
		return false;

	if ( node.at != b->at )
		return false;

	if ( node.size != b->size )
		return false;

	return true;
}

String BoundingNodeGd::node_id() const
{
	String ret = itos(node.at.x) + String(",") +
		         itos(node.at.x) + String(",") +
		         itos(node.at.x) + String(",") +
		         itos(node.size);
	return ret;
}



}

