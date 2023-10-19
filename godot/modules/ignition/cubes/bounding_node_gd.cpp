
#include "bounding_node_gd.h"
#include "marching_cubes_dual_gd.h"

#include "core/templates/rb_map.h"

namespace Ign
{

void BoundingNodeGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("create_adjacent_node", "dx", "dy", "dz"), &BoundingNodeGd::create_adjacent_node );
	ClassDB::bind_method( D_METHOD("equals_to", "other_node"),                &BoundingNodeGd::equals_to );
	ClassDB::bind_method( D_METHOD("get_node_id"),                            &BoundingNodeGd::get_node_id );
	ClassDB::bind_method( D_METHOD("get_adjacent_node_id", "dx", "dy", "dz"), &BoundingNodeGd::get_adjacent_node_id );
	ClassDB::bind_method( D_METHOD("get_center", "surface"),                  &BoundingNodeGd::get_center );
	ClassDB::bind_method( D_METHOD("get_size", "surface"),                    &BoundingNodeGd::get_size );
	ClassDB::bind_method( D_METHOD("get_hash"),                               &BoundingNodeGd::get_hash );

	ClassDB::bind_method( D_METHOD("contains_point", "surface", "s3"),        &BoundingNodeGd::contains_point );

	ClassDB::bind_static_method( "BoundingNodeGd", D_METHOD("split_into_clusters", "nodes"),    &BoundingNodeGd::split_into_clusters );
	ClassDB::bind_static_method( "BoundingNodeGd", D_METHOD("cluster_contains_point", "surface", "bounding_nodes"), &BoundingNodeGd::cluster_contains_point );
}

BoundingNodeGd::BoundingNodeGd()
	: RefCounted()
{
}

BoundingNodeGd::~BoundingNodeGd()
{
}

Ref<BoundingNodeGd> BoundingNodeGd::create_adjacent_node( int dx, int dy, int dz ) const
{
	Ref<BoundingNodeGd> ret;
	ret.instantiate();
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
		         itos(node.at.y) + String(",") +
		         itos(node.at.z) + String(",") +
		         itos(node.size);
	return ret;
}

String BoundingNodeGd::get_adjacent_node_id( int dx, int dy, int dz ) const
{
	String ret = itos(node.at.x + dx*node.size) + String(",") +
                 itos(node.at.y + dy*node.size) + String(",") +
                 itos(node.at.z + dz*node.size) + String(",") +
                 itos(node.size);
	return ret;
}

Ref<Se3Ref> BoundingNodeGd::get_center( const Ref<MarchingCubesDualGd> & surface ) const
{
	const VectorInt c               = node.center();
	const MarchingCubesDual & cubes = surface->cubes;
	const Vector3d at               = cubes.at_in_source( c );
	const SE3 se3                   = cubes.se3_in_point( at );

	Ref<Se3Ref> ret_se3;
	ret_se3.instantiate();
	ret_se3->se3 = se3;
	return ret_se3;
}

real_t BoundingNodeGd::get_size( const Ref<MarchingCubesDualGd> & surface ) const
{
	const MarchingCubesDual & cubes = surface->cubes;

	const Float ret = node.node_size( &cubes );
	return ret;
}

String BoundingNodeGd::get_hash() const
{
	const uint64_t h = node.hash.state();
	const String s_hash = uitos( h );
	return s_hash;
}

bool BoundingNodeGd::contains_point( const Ref<MarchingCubesDualGd> & surface, const Ref<Se3Ref> & se3 ) const
{
	const MarchingCubesDual * surf = &(surface.ptr()->cubes);
	const Vector3d & r = se3.ptr()->se3.r();
	const bool ret = node.contains_point( surf, r );
	return ret;
}

//static void print_map( const String & header, const RBMap<String, Ref<BoundingNodeGd> > & map )
//{
//	print_line( header );
//	String stri;
//	for (const KeyValue<String, Ref<BoundingNodeGd> > & E : map)
//	{
//		const Ref<BoundingNodeGd> & node_ref = E.value;
//		const String id = node_ref.ptr()->get_node_id();
//		stri += "(" + id + "), ";
//	}
//	print_line( stri );
//}
//
//static void print_vector( const String & header, const Vector<Ref<BoundingNodeGd> > & vec )
//{
//	print_line( header );
//	String stri;
//	const int qty = vec.size();
//	for ( int i=0; i<qty; i++ )
//	{
//		const Ref<BoundingNodeGd> & node_ref = vec[i];
//		const String id = node_ref.ptr()->get_node_id();
//		stri += "(" + id + "), ";
//	}
//	print_line( stri );
//}

Array BoundingNodeGd::split_into_clusters( const Array & bounding_nodes )
{
	RBMap<String, Ref<BoundingNodeGd> >  remaining_nodes;
	RBMap<String, Ref<BoundingNodeGd> >  destination_nodes;
	Vector<Ref<BoundingNodeGd> > last_added, newly_added;
	Array destination_array, result;

	const int total_qty = bounding_nodes.size();
	for ( int i=0; i<total_qty; i++ )
	{
		const Variant & v = bounding_nodes.get( i );
		const Ref<BoundingNodeGd> node_ref = v;
		const BoundingNodeGd * node = node_ref.ptr();
		const String id = node->get_node_id();
		const bool has = remaining_nodes.has( id );
		if ( !has )
			remaining_nodes.insert( id, node_ref );
	}

	//print_line( "entered clusters" );

	while ( !remaining_nodes.is_empty() )
	{
		// First step prime destination_nodes and last_added.
		const Ref<BoundingNodeGd> node_ref = remaining_nodes.back()->get();
		const BoundingNodeGd * node = node_ref.ptr();
		const String id = node->get_node_id();
		remaining_nodes.erase( id );

		last_added.clear();
		destination_nodes.clear();
		last_added.push_back( node_ref );
		destination_nodes.insert( id, node_ref );

		//print_line( "Initial id: ", id );

		while ( !last_added.is_empty() )
		{
			//print_map( "destination: ", destination_nodes );
			//print_vector( "last_added: ", last_added );
			//print_map( "remaining: ", remaining_nodes );

			newly_added.clear();
			const int qty = last_added.size();
			for ( int i=0; i<qty; i++ )
			{
				const Ref<BoundingNodeGd> node_ref = last_added[i];
				const BoundingNodeGd * node = node_ref.ptr();
				// Check front, back, top, bottom, left, right.
				for ( int x=0; x<3; x++ )
				{
					const int dx = x-1;
					for ( int y=0; y<3; y++ )
					{
						const int dy = y-1;
						for ( int z=0; z<3; z++ )
						{
							const int dz = z-1;
							int sum = 0;
							if (dx != 0)
								sum += 1;
							if (dy != 0)
								sum += 1;
							if (dz != 0)
								sum += 1;
							if (sum != 1)
								continue;
							const String id = node->get_adjacent_node_id( dx, dy, dz );
							const bool remaining_has   = remaining_nodes.has( id );
							const bool destination_has = destination_nodes.has( id );
							//print_line( "(" + id + "): in_remaining: " +
							//	        (remaining_has ? "true" : "false") + "; in_dest: " +
							//            (destination_has ? "true" : "false" ) );

							if ( remaining_has && (!destination_has) )
							{
								const Ref<BoundingNodeGd> node_ref_to_add = remaining_nodes[id];
								remaining_nodes.erase( id );
								destination_nodes.insert( id, node_ref_to_add );
								newly_added.push_back( node_ref_to_add );
							}
						}
					}
				}
			}

			last_added = newly_added;
		}

		//print_map( "final_destination: ", destination_nodes );

		destination_array.clear();
		for (const KeyValue<String, Ref<BoundingNodeGd> > & E : destination_nodes)
		{
			const Ref<BoundingNodeGd> node_ref = E.value;
			destination_array.push_back( node_ref );
		}

		//for ( RBMap<String, const Ref<BoundingNodeGd> *>::Iterator it = destination_nodes.begin();
		//	  it != destination_nodes.end(); it++ )
		//{
		//	const Ref<BoundingNodeGd> * node_ref = it->value;
		//	destination_array.push_back( &node_ref );
		//}

		result.push_back( destination_array );
	}

	return result;
}

bool BoundingNodeGd::cluster_contains_point( const Ref<MarchingCubesDualGd> & surface, const Array & bounding_nodes, const Ref<Se3Ref> & se3 )
{
	const int qty = bounding_nodes.size();
	for ( int i=0; i<qty; i++ )
	{
		const Variant & v = bounding_nodes.get( i );
		const Ref<BoundingNodeGd> node_ref = v;
		const BoundingNodeGd * node = node_ref.ptr();

		const bool contains = node->contains_point( surface, se3 );
		if ( contains )
			return true;
	}

	return false;
}






}





