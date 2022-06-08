
#include "marching_cubes_spherical_rebuild_strategy_gd.h"

namespace Ign
{

void MarchingCubesSphericalRebuildStrategyGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("initialize", "radius", "scaler"), &MarchingCubesSphericalRebuildStrategyGd::initialize );
}

MarchingCubesSphericalRebuildStrategyGd::MarchingCubesSphericalRebuildStrategyGd()
	: MarchingCubesRebuildStrategyGd()
{
	strategy = &spherical_strategy;
}

MarchingCubesSphericalRebuildStrategyGd::~MarchingCubesSphericalRebuildStrategyGd()
{
}

void MarchingCubesSphericalRebuildStrategyGd::initialize( real_t radius, const Ref<DistanceScalerRef> & scaler )
{
	DistanceScaler * s = const_cast<DistanceScaler *>( &(scaler.ptr()->scaler) );
	spherical_strategy.initialize( radius, s );
}



}




