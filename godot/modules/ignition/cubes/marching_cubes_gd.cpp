
#include "marching_cubes_gd.h"


namespace Ign
{

void MarchingCubesGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_source_transform", "se3"),          &MarchingCubesGd::set_source_transform );
	ClassDB::bind_method( D_METHOD("subdivide_source", "source", "scaler"), &MarchingCubesGd::subdivide_source, Variant::BOOL );
	ClassDB::bind_method( D_METHOD("faces"),                                &MarchingCubesGd::faces,            Variant::POOL_VECTOR3_ARRAY );
}

MarchingCubesGd::MarchingCubesGd()
	: Reference()
{
}

MarchingCubesGd::~MarchingCubesGd()
{
}

void MarchingCubesGd::set_source_transform( const Ref<Se3Ref> & se3 )
{
	const SE3 & se3_inst = se3.ptr()->se3;
	cubes.set_source_transform( se3_inst );
}

bool MarchingCubesGd::subdivide_source( const Ref<VolumeSourceGd> & source, const Ref<DistanceScalerRef> & scaler )
{
	VolumeSource * volume_source = source.ptr()->source;
	const DistanceScalerRef * distance_scaler_ref = scaler.ptr();
	const DistanceScaler * distance_scaler = (distance_scaler_ref != nullptr) ? &(distance_scaler_ref->scaler) : nullptr;

	const bool ret = cubes.subdivide_source( volume_source, distance_scaler );
	return ret;
}

PoolVector3Array MarchingCubesGd::faces() const
{
	const Vector<Face3> & f = cubes.faces();
	PoolVector3Array ret;
	const int qty = f.size();
	const int pts_qty = qty * 3;
	ret.resize( pts_qty );

	int ind = 0;
	for ( int i=0; i<qty; i++ )
	{
		const Face3 & face = f.ptr()[i];
		ret.set( ind,   face.vertex[0] );
		ret.set( ind+1, face.vertex[1] );
		ret.set( ind+2, face.vertex[2] );
		ind += 3;
	}

	return ret;
}




}




