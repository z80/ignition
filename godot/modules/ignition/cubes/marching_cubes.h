
#include "data_types.h"
#include "vector3d.h"

namespace Ign
{

class VolumeSource;

class IndexedVertex
{
public:
	IndexedVertex();
	~IndexedVertex();
	IndexedVertex( const IndexedVertex & inst );

	const IndexedVertex & operator=( const IndexedVertex & inst );
	const bool operator==( const IndexedVertex & other ) const;
	const bool operator<( const IndexedVertex & other ) const;
	int indices[3];
};

struct Cube
{
	IndexedVertex verts[8];
};

class MarchingCubes
{
public:
	MarchingCubes();
	~MarchingCubes();

	void subdivide_source( VolumeSource * source );


	Float iso_level;
	Float eps;
	Float step;
private:
	Vector3d interpolate( const Vector3d & v0, const Vector3d & v1, const Float val0, const Float val1 ) const;
};



}







