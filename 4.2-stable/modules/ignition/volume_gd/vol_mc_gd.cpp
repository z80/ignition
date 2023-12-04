
#include "vol_mc_gd.h"

#include "vector3d.h"
#include "vol_geometry_gd.h"
#include "vol_source_gd.h"
#include "vol_mc.h"

namespace Ign
{

void VolMcGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("build_surface", "center", "physics_direct_space_state_3d", "size", "resolution"), &VolMcGd::build_surface );
}

VolMcGd::VolMcGd()
	: RefCounted()
{
}

VolMcGd::~VolMcGd()
{
}

Array VolMcGd::build_surface( const Vector3 & center, Object * physics_direct_space_state_3d, real_t size, int resolution )
{
	PhysicsDirectSpaceState3D * state = Object::cast_to<PhysicsDirectSpaceState3D>( physics_direct_space_state_3d );

	const Float half_size = size * 0.5;
	const Vector3d geometry_center = Vector3d( center.x - half_size, center.y - half_size, center.z - half_size );
	VolGeometryGd geometry( geometry_center );
	VolSourceGd   source( state );

	VolMc mc;
	const bool ok = mc.build_surface( &geometry, &source, size, resolution );

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


