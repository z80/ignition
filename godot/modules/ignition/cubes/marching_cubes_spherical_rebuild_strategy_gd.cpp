
#include "marching_cubes_spherical_rebuild_strategy_gd.h"

namespace Ign
{

void MarchingCubesSphericalRebuildStrategyGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("initialize", "radius", "scaler"), &MarchingCubesSphericalRebuildStrategyGd::initialize );

	ClassDB::bind_method( D_METHOD("set_rebuild_angle", "ang"), &MarchingCubesSphericalRebuildStrategyGd::set_rebuild_angle );
	ClassDB::bind_method( D_METHOD("get_rebuild_angle"),        &MarchingCubesSphericalRebuildStrategyGd::get_rebuild_angle, Variant::REAL );

	ADD_PROPERTY( PropertyInfo( Variant::REAL, "rebuild_angle" ), "set_rebuild_angle", "get_rebuild_angle" );
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
		s = scaler->scaler_base;
	else
		s = nullptr;
	spherical_strategy.initialize( radius, s );
}

void MarchingCubesSphericalRebuildStrategyGd::set_rebuild_angle( real_t ang )
{
	spherical_strategy.set_rebuild_angle( ang );
}

real_t MarchingCubesSphericalRebuildStrategyGd::get_rebuild_angle() const
{
	const real_t ret = spherical_strategy.get_rebuild_angle();
	return ret;
}



}




