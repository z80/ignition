
#ifndef __MARCHING_CUBES_GD_H_
#define __MARCHING_CUBES_GD_H_

#include "marching_cubes.h"
#include "core/reference.h"
#include "se3_ref.h"
#include "volume_source_gd.h"
#include "distance_scaler_ref.h"


class Node;

namespace Ign
{

class MarchingCubesGd: public Reference
{
	GDCLASS(MarchingCubesGd, Reference);
protected:
	static void _bind_methods();

public:
	MarchingCubesGd();
	~MarchingCubesGd();

	void set_source_transform( const Ref<Se3Ref> & se3 );
	bool subdivide_source( const Ref<VolumeSourceGd> & source, const Ref<DistanceScalerRef> & scaler = Ref<DistanceScalerRef>() );
	PoolVector3Array faces() const;
	void apply_to_mesh( Node * mesh_instance );

public:
	MarchingCubes cubes;

	PoolVector3Array vertices, normals;
	PoolRealArray    tangents;
};




}





#endif







