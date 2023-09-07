
#ifndef __VOL_MC_H_
#define __VOL_MC_H_

#include "data_types.h"
#include "vol_data_types.h"
#include "vector3d.h"

#include "core/templates/vector.h"


namespace Ign
{

class VolMcNode;
class VolGeometry;
class VolSource;

class VolMc
{
public:
	VolMc();
	~VolMc();

	bool build_surface( const VolGeometry * geometry, const VolSource * source, Float size=0.1, Integer resolution=2 );

	const Vector<VolFace> & faces() const;

private:
	Float   _size;
	Integer _resolution;
	Float   _scale;

	void _compute_scale();
	void _traverse_nodes_brute_force( const VolGeometry * geometry, const VolSource * source );
	bool _compute_node_values( VolMcNode & node, Vector3d * intersection_pts, const VolGeometry * geometry, const VolSource * source, bool * pts_assigned );
	bool _assign_edge_values( const Vector3d & a, const Vector3d & b, Float & va, Float & vb, Vector3d & at, const VolSource * source ) const;

	void _create_faces( const VolMcNode & node, const Vector3d * intersection_pts, const bool * pts_assigned );


	Vector<VolFace> _faces;
};


}

#endif


