
#include "vol_source_gd.h"
#include "core/string/print_string.h"

namespace Ign
{

VolSourceGd::VolSourceGd( PhysicsDirectSpaceState3D * state )
	: VolSource(),
	  _state( state )
{
}

VolSourceGd::~VolSourceGd()
{
}

bool VolSourceGd::intersects( const Vector3d & start, const Vector3d & end, Float & v_start, Float & v_end, Vector3d & at ) const
{
	if ( _state == nullptr )
	{
		print_line( String( "VolSourceGd: invalid physics direct space state 3d" ) );
		return false;
	}

	PhysicsDirectSpaceState3D::RayParameters args;
	PhysicsDirectSpaceState3D::RayResult     res;

	args.collide_with_areas  = true;
	args.collide_with_bodies = true;
	args.hit_back_faces      = true;
	args.hit_from_inside     = false;
	args.from                = Vector3( start.x_, start.y_, start.z_ );
	args.to                  = Vector3( end.x_,   end.y_,   end.z_ );
	//args.pick_ray            = true;

	bool ok = _state->intersect_ray( args, res );
	if ( !ok )
	{
		args.from = Vector3( end.x_,   end.y_,   end.z_ );
		args.to   = Vector3( start.x_, start.y_, start.z_ );
		ok = _state->intersect_ray( args, res );
	}


	if ( ok )
	{
		at = Vector3d( res.position.x, res.position.y, res.position.z );
		const Vector3d norm = Vector3d( res.normal.x, res.normal.y, res.normal.z );

		const Vector3d a = end - start;
		const bool positive = a.DotProduct( norm );
		const Float ta = a.DotProduct( at - start ) / a.DotProduct( a );
		v_start = -ta;
		v_end   = 1.0 - ta;
		if ( !positive )
		{
			v_start = -v_start;
			v_end   = -v_end;
		}
	}

	return ok;
}








}




