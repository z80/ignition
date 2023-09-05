
#include "vol_mc_gd.h"

#include "vector3d.h"
#include "vol_geometry_gd.h"
#include "vol_source_gd.h"
#include "vol_mc.h"

namespace Ign
{

void VolMcGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("build_surface", "center", "physics_direct_space_state_3d", "scale", "subdivisions"), &VolMcGd::build_surface );
}

VolMcGd::VolMcGd()
{
}

VolMcGd::~VolMcGd()
{
}

Array VolMcGd::build_surface( const Vector3 & center, Object * physics_direct_space_state_3d, real_t scale, int subdivisions )
{
	PhysicsDirectSpaceState3D * state = Object::cast_to<PhysicsDirectSpaceState3D>( physics_direct_space_state_3d );
	VolGeometryGd geometry( Vector3d( center.x, center.y, center.z ) );
	VolSourceGd   source( state );

	VolMc mc;
	const bool ok = mc.build_surface( &geometry, &source, scale, subdivisions );

	Array ret;
	const Vector<VolFace> & faces = mc.faces();
	const int qty = faces.size();
	for ( int i=0; i<qty; i++ )
	{
		const VolFace & f = faces.get( i );
		for ( int j=0; j<3; j++ )
		{
			const Vector3d & a = f.vertices[j];
			const Vector3 vert( a.x_, a.y_, a.z_ );
			ret.push_back( vert );
		}
	}
	return ret;
}


}


