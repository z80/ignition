
#include "marching_cubes_dual_node_gd.h"
#include "marching_cubes_dual.h"
#include "marching_cubes_dual_node.h"


namespace Ign
{

void MarchingCubesDualNodeGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("intersect_with_segment", "start", "end"), &MarchingCubesDualNodeGd::intersect_with_segment, Variant::ARRAY );
	ClassDB::bind_method( D_METHOD("intersect_with_ray", "start", "dir"),     &MarchingCubesDualNodeGd::intersect_with_ray,     Variant::ARRAY );
	ClassDB::bind_method( D_METHOD("hierarchy_path"),                         &MarchingCubesDualNodeGd::hierarchy_path,         Variant::STRING );
	ClassDB::bind_method( D_METHOD("contains_point", "at"),                   &MarchingCubesDualNodeGd::contains_point,         Variant::BOOL );
	ClassDB::bind_method( D_METHOD("center_vector"),                          &MarchingCubesDualNodeGd::center_vector,          Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("node_size"),                              &MarchingCubesDualNodeGd::node_size,              Variant::REAL );
	ClassDB::bind_method( D_METHOD("se3_in_point", "at"),                     &MarchingCubesDualNodeGd::se3_in_point,           Variant::OBJECT );
	ClassDB::bind_method( D_METHOD("transform_in_point", "at"),               &MarchingCubesDualNodeGd::transform_in_point,     Variant::TRANSFORM );
	ClassDB::bind_method( D_METHOD("hash"),                                   &MarchingCubesDualNodeGd::hash,                   Variant::STRING );
	ClassDB::bind_method( D_METHOD("asset_transform", "source_se3", "se3", "scaler"), &MarchingCubesDualNodeGd::asset_transform,        Variant::TRANSFORM );
	ClassDB::bind_method( D_METHOD("at"),                                     &MarchingCubesDualNodeGd::at,                     Variant::ARRAY );
	ClassDB::bind_method( D_METHOD("size"),                                   &MarchingCubesDualNodeGd::size,                   Variant::INT );
}

MarchingCubesDualNodeGd::MarchingCubesDualNodeGd()
{
	cubes = nullptr;
	node  = nullptr;
}

MarchingCubesDualNodeGd::~MarchingCubesDualNodeGd()
{
}

Array MarchingCubesDualNodeGd::intersect_with_segment( const Vector3 & start, const Vector3 & end )
{
	ret_array.clear();

	if ( ( cubes == nullptr ) || ( node == nullptr ) )
	{
		ret_array.push_back( false );
		return ret_array;
	}

	const Vector3d s( start.x, start.y, start.z );
	const Vector3d e( end.x, end.y, end.z );
	Vector3d at, norm;
	const bool ok = node->intersect_with_segment( cubes, s, e, at, norm );
	ret_array.push_back( ok );
	if ( ok )
	{
		ret_array.push_back( Vector3( at.x_, at.y_, at.z_ ) );
		ret_array.push_back( Vector3( norm.x_, norm.y_, norm.z_ ) );
	}
	return ret_array;
}

Array MarchingCubesDualNodeGd::intersect_with_ray( const Vector3 & start, const Vector3 & dir )
{
	ret_array.clear();

	if ( ( cubes == nullptr ) || ( node == nullptr ) )
	{
		ret_array.push_back( false );
		return ret_array;
	}

	const Vector3d s( start.x, start.y, start.z );
	const Vector3d d( dir.x, dir.y, dir.z );
	Vector3d at, norm;
	const bool ok = node->intersect_with_ray( cubes, s, d, at, norm );
	ret_array.push_back( ok );
	if ( ok )
	{
		ret_array.push_back( Vector3( at.x_, at.y_, at.z_ ) );
		ret_array.push_back( Vector3( norm.x_, norm.y_, norm.z_ ) );
	}
	return ret_array;
}

String MarchingCubesDualNodeGd::hierarchy_path() const
{
	if ( ( cubes == nullptr ) || ( node == nullptr ) )
	{
		return String();
	}

	const String ret = node->hierarchy_path();
	return ret;
}

bool MarchingCubesDualNodeGd::contains_point( const Vector3 & at ) const
{
	if ( ( cubes == nullptr ) || ( node == nullptr ) )
	{
		return false;
	}

	const Vector3d v( at.x, at.y, at.z );
	const bool ret = node->contains_point( cubes, v );
	return ret;
}

Vector3 MarchingCubesDualNodeGd::center_vector() const
{
	if ( ( cubes == nullptr ) || ( node == nullptr ) )
	{
		return Vector3();
	}

	const Vector3d v = node->center_vector( cubes );
	const Vector3 ret( v.x_, v.y_, v.z_ );
	return ret;
}

real_t MarchingCubesDualNodeGd::node_size() const
{
	if ( ( cubes == nullptr ) || ( node == nullptr ) )
	{
		return 0.0;
	}

	const real_t ret = node->node_size( cubes );
	return ret;
}

Ref<Se3Ref> MarchingCubesDualNodeGd::se3_in_point( const Vector3 & at ) const
{
	Ref<Se3Ref> se3;
	se3.instance();

	if ( ( cubes == nullptr ) || ( node == nullptr ) )
	{
		return se3;
	}

	const Vector3d at_d( at.x, at.y, at.z );
	se3->se3 = cubes->se3_in_point( at_d );
	return se3;
}

Transform MarchingCubesDualNodeGd::transform_in_point( const Vector3 & at ) const
{
	if ( ( cubes == nullptr ) || ( node == nullptr ) )
	{
		return Transform();
	}

	const Vector3d at_d( at.x, at.y, at.z );
	const SE3 se3 = cubes->se3_in_point( at_d );
	const Transform t( Basis( Quat( se3.q_.x_, se3.q_.y_, se3.q_.z_, se3.q_.w_ ) ), Vector3( se3.r_.x_, se3.r_.y_, se3.r_.z_ ) );
	return t;
}

String MarchingCubesDualNodeGd::hash() const
{
	const uint64_t h = node->hash.state();
	const String s_hash = uitos( h );
	return s_hash;
}

Transform MarchingCubesDualNodeGd::asset_transform( const Ref<Se3Ref> & src_se3, const Ref<Se3Ref> & asset_se3, const Ref<DistanceScalerRef> & scaler ) const
{
	if ( ( cubes == nullptr ) || ( node == nullptr ) )
	{
		return Transform();
	}

	const SE3 source_se3 = (src_se3.ptr() == nullptr) ? SE3() : src_se3->se3;
	const SE3 se3        = (asset_se3.ptr() == nullptr) ? SE3() : asset_se3->se3;
	const DistanceScalerBase * s;
	if ( scaler.ptr() != nullptr )
		s = &(scaler->scaler);
	else
		s = nullptr;
	const SE3 ret_se3 = cubes->asset_se3( source_se3, se3, s );
	const Transform t = ret_se3.transform();

	return t;
}

Array MarchingCubesDualNodeGd::at()
{
	ret_array.clear();
	if ( ( cubes == nullptr ) || ( node == nullptr ) )
	{
		return ret_array;
	}

	ret_array.push_back( node->at.x );
	ret_array.push_back( node->at.y );
	ret_array.push_back( node->at.z );

	return ret_array;
}

int MarchingCubesDualNodeGd::size() const
{
	if ( ( cubes == nullptr ) || ( node == nullptr ) )
	{
		return 0;
	}

	const int ret = node->size;
	return ret;
}




}





