
#include "bounding_node_gd.h"


namespace Ign
{

void BoundingNodeGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("create_adjacent_node", "dx", "dy", "dz"), &BoundingNodeGd::create_adjacent_node, Variant::OBJECT );
	ClassDB::bind_method( D_METHOD("equals_to", "other_node"),                &BoundingNodeGd::equals_to,            Variant::BOOL );
	ClassDB::bind_method( D_METHOD("get_node_id"),                            &BoundingNodeGd::get_node_id,          Variant::STRING );
	ClassDB::bind_method( D_METHOD("get_adjacent_node_id", "dx", "dy", "dz"), &BoundingNodeGd::get_adjacent_node_id, Variant::STRING );
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

	if ( node.at != b->node.at )
		return false;

	if ( node.size != b->node.size )
		return false;

	return true;
}

String BoundingNodeGd::get_node_id() const
{
	String ret = itos(node.at.x) + String(",") +
		         itos(node.at.x) + String(",") +
		         itos(node.at.x) + String(",") +
		         itos(node.size);
	return ret;
}

String BoundingNodeGd::get_adjacent_node_id( int dx, int dy, int dz ) const
{
	String ret = itos(node.at.x + dx*node.size) + String(",") +
                 itos(node.at.x + dy*node.size) + String(",") +
                 itos(node.at.x + dz*node.size) + String(",") +
                 itos(node.size);
	return ret;
}




}

