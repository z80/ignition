
#ifndef __MARCHING_CUBES_H_
#define __MARCHING_CUBES_H_




#include "data_types.h"
#include "vector3d.h"
//#include "hashfuncs.h"
#include "core/vector.h"
#include "core/math/face3.h"

#include "se3.h"


#include <vector>
#include <set>
#include <map>

#include "cube_types.h"

namespace Ign
{

class VolumeSource;
class MaterialSource;
class DistanceScaler;





/*struct MarchingNodeHasher
{
    static _FORCE_INLINE_ uint32_t hash( const MarchingNode & n )
    {
        const uint8_t * c = reinterpret_cast<const uint8_t *>( &(n.at.x) );
        uint32_t h = hash_djb2_buffer( c, sizeof(int) );

        c = reinterpret_cast<const uint8_t *>( &(n.at.y) );
        h = hash_djb2_buffer( c, sizeof(int), h );

        c = reinterpret_cast<const uint8_t *>( &(n.at.z) );
        h = hash_djb2_buffer( c, sizeof(int), h );

        c = reinterpret_cast<const uint8_t *>( &(n.size) );
        h = hash_djb2_buffer( c, sizeof(int), h );

        return p_string.hash();
    }
}*/

/*struct MarchingNodeCompare
{
    static bool operator<( const MarchingNode & a, const MarchingNode & b ) const
    {
        if ( a.at.x < b.at.x )
            return true;
        else if ( a.at.y < b.at.y )
            return true;
        else if ( a.at.z < b.at.z )
            return true;
        else if ( a.size < b.size )
            return true;

        return false;
    }

	static bool operator==( const MarchingNode & a, const MarchingNode & b ) const
	{
		if ( a.at.x != b.at.x )
			return false;
		else if ( a.at.y != b.at.y )
			return false;
		else if ( a.at.z != b.at.z )
			return false;
		else if ( a.size != b.size )
			return false;

		return true;
	}
};*/



class MarchingCubes
{
public:
    MarchingCubes();
    ~MarchingCubes();

    void set_source_transform( const SE3 & se3 );
    bool subdivide_source( VolumeSource * source, const DistanceScaler * scaler = nullptr );

	const std::set<int>        & materials() const;
	const std::vector<Vector3> & vertices( int material_ind );
	const std::vector<Vector3> & normals( int material_ind );
	const std::vector<real_t>  & tangents( int material_ind );

	const Transform source_transform( const DistanceScaler * scaler = nullptr) const;


    Float    node_size( int level ) const;
    Vector3d at( const VectorInt & at_i, const DistanceScaler * scaler=nullptr ) const;
	Vector3d node_center( const MarchingNode & node, const DistanceScaler * scaler=nullptr ) const;

    



    Float iso_level;
    Float eps;
    // This is the smallest cube size.
    Float step;
    // Number of scale up levels.
    int   levels_qty;
	// Max allowed number of nodes.
	int   max_nodes_qty;

    // Source transform.
    // Points to probe first are sent through this transformation.
    SE3 source_se3;
	SE3 inverted_source_se3;
private:
	void find_subdivision_levels( VolumeSource * source );
	bool find_surface( VolumeSource * source, const DistanceScaler * scaler, MarchingNode & surface_node );
	void compute_node_values( MarchingNode & node, VolumeSource * source, const DistanceScaler * scaler );
	MarchingNode step_towards_surface( const MarchingNode & node, VolumeSource * source, const DistanceScaler * scaler );

	Vector3d interpolate( const Vector3d & v0, const Vector3d & v1, const Float val0, const Float val1 ) const;

    // For BFS search.
    // 26 Node neighbors.
    void add_node_neighbors( const MarchingNode & node, VolumeSource * source, const DistanceScaler * scaler, int & nodes_qty );

    // Create faces.
    void create_faces( const MarchingNode & node, int material_index = -1 );

	// Compute value with reusing pre-computed values.
	Float value_at( VolumeSource * source, const VectorInt & vector_int, const Vector3d & at );
	int   material_at( VolumeSource * source, const VectorInt & vector_int, const Vector3d & at );

	// Store face normal for node edge.
	void append_normal( const NodeEdgeInt & edge, const Vector3d & n );
    
    typedef std::set<MarchingNode>                 MarchingSet;
    typedef std::set<MarchingNode>::iterator       MarchingSetIterator;
	typedef std::set<MarchingNode>::const_iterator MarchingSetConstIterator;

	typedef std::map<VectorInt, Float>                 ValuesMap;
	typedef std::map<VectorInt, Float>::iterator       ValuesMapIterator;
	typedef std::map<VectorInt, Float>::const_iterator ValuesMapConstIterator;

	typedef std::map<VectorInt, int>                   MaterialsMap;
	typedef std::map<VectorInt, int>::iterator         MaterialsMapIterator;
	typedef std::map<VectorInt, int>::const_iterator   MaterialsMapConstIterator;

	typedef std::map<NodeEdgeInt, NormalsAndQty>                 NormalsMap;
	typedef std::map<NodeEdgeInt, NormalsAndQty>::iterator       NormalsMapIterator;
	typedef std::map<NodeEdgeInt, NormalsAndQty>::const_iterator NormalsMapConstIterator;

	typedef std::set<int>                 MaterialsSet;
	typedef std::set<int>::iterator       MaterialsSetIterator;
	typedef std::set<int>::const_iterator MaterialsSetConstIterator;

    MarchingSet _all_nodes;
    MarchingSet _recently_added_nodes;
    MarchingSet _new_candidates;

	std::vector<NodeFace> _all_faces;
	// Results for applying to meshes.
	std::vector<Vector3>  _verts;
	std::vector<Vector3>  _norms;
	std::vector<real_t>   _tangs;
	std::vector<int>      _materials;

	// For returning results.
	std::vector<Vector3>  _ret_verts;
	std::vector<Vector3>  _ret_norms;
	std::vector<real_t>   _ret_tangs;

	// In order to not compute values a few times.
	ValuesMap     _values_map;
	MaterialsMap  _materials_map;

	// In order to compute normals.
	NormalsMap    _normals_map;

	// Materials set in order to know how many materials in total actually are involved.
	MaterialsSet _materials_set;
};



}








#endif







