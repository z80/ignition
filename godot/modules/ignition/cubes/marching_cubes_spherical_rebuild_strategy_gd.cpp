
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

void MarchingCubesSphericalRebuildStrategyGd::initialize( real_t radius, const Ref<DistanceScalerBaseRef> & scaler )
{
	const DistanceScalerBase * s;
	if ( scaler.ptr() != nullptr )
		s = scaler->scaler;
	else
		s = nullptr;
	spherical_strategy.initialize( radius, s );
}



}




