
#ifndef __MARCHING_CUBES_DUAL_H_
#define __MARCHING_CUBES_DUAL_H_




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
class DistanceScalerBase;

class MarchingCubesDualNode;
class MarchingCubesDualCell;

struct MaterialWithPriorityDual
{
	int material;
	int priority;
};


// This one is for holding the correspondinces of
// which octreenodes have contributed to dual cell creation.
// After creating faces it is easier to determine what faces are inside
// of what octree nodes.
class DualCellOctreeNodePair
{
public:
	MarchingCubesDualNode * node;
	MarchingCubesDualCell * cell;

	DualCellOctreeNodePair()
	{
		node = nullptr;
		cell = nullptr;
	}

	~DualCellOctreeNodePair()
	{
	}

	DualCellOctreeNodePair( const DualCellOctreeNodePair & inst )
	{
		*this = inst;
	}

	const DualCellOctreeNodePair & operator=( const DualCellOctreeNodePair & inst )
	{
		if ( this != &inst )
		{
			node = inst.node;
			cell = inst.cell;
		}
		return *this;
	}

	DualCellOctreeNodePair( MarchingCubesDualCell * c, MarchingCubesDualNode * n )
	{
		cell = c;
		node = n;
	}
};

bool operator<( const DualCellOctreeNodePair & a, const DualCellOctreeNodePair & b );



class MarchingCubesDual
{
public:
    MarchingCubesDual();
    ~MarchingCubesDual();

    void set_source_transform( const SE3 & se3 );
    bool subdivide_source( Float bounding_radius, VolumeSource * source, const DistanceScalerBase * scaler = nullptr );

	// These two are called by nodes.
	void set_split_precision( Float rel_diff );
	Float get_split_precision() const;
	bool should_split( MarchingCubesDualNode * node, VolumeSource * source, const DistanceScalerBase * scaler );
	MarchingCubesDualNode * create_node();
	MarchingCubesDualCell * create_dual_cell();


    const std::set<int>        & materials() const;
    const std::vector<Vector3> & vertices( int material_ind );
    const std::vector<Vector3> & normals( int material_ind );
    const std::vector<real_t>  & tangents( int material_ind );

    const std::vector<Vector3> & collision_faces( const Float dist, const DistanceScalerBase * scaler = nullptr );

    const Transform source_transform( const DistanceScalerBase * scaler = nullptr) const;

    Float    node_size_min( const MarchingCubesDualNode * node ) const;
	Float    node_size_max( const MarchingCubesDualNode * node ) const;
	Vector3d at_in_source_scaled( const VectorInt & at_i, const DistanceScalerBase * scaler=nullptr ) const;
    Vector3d at_in_source( const VectorInt & at_i ) const;

    
	// Debug methods.
	int  get_nodes_qty() const;
	void get_node( int node_ind, Vector3d * corners ) const;
	int  get_node_parent( int node_ind ) const;

	int  get_dual_cells_qty() const;
	void get_dual_cell( int cell_ind, Vector3d * corners ) const;
	


    Float iso_level;
	Float max_rel_diff;
    Float eps;
    // This is the smallest cube size.
    Float step;
    // Max allowed number of nodes.
    int   max_nodes_qty;

    // Source transform.
    // Points to probe first are sent through this transformation.
    SE3      source_se3;
    SE3      inverted_source_se3;

public:
	void cleanup_nodes();

    int find_subdivision_levels( Float bounding_radius, VolumeSource * source );
    //bool find_surface( VolumeSource * source, const DistanceScalerBase * scaler, MarchingNode & surface_node );
    void compute_node_values( MarchingCubesDualNode & node, VolumeSource * source, const DistanceScalerBase * scaler );
    //MarchingNode step_towards_surface( const MarchingCubesDualNode & node, VolumeSource * source, const DistanceScalerBase * scaler );


	// Dual grid generation methods.
	void node_proc( const MarchingCubesDualNode * n, VolumeSource * source, const DistanceScalerBase * scaler );
	void face_proc_xy( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, VolumeSource * source, const DistanceScalerBase * scaler );
	void face_proc_zy( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, VolumeSource * source, const DistanceScalerBase * scaler );
	void face_proc_xz( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, VolumeSource * source, const DistanceScalerBase * scaler );
	void edge_proc_x( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3, VolumeSource * source, const DistanceScalerBase * scaler );
	void edge_proc_y( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3, VolumeSource * source, const DistanceScalerBase * scaler );
	void edge_proc_z( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3, VolumeSource * source, const DistanceScalerBase * scaler );
	void vert_proc( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3,
		            const MarchingCubesDualNode * n4, const MarchingCubesDualNode * n5, const MarchingCubesDualNode * n6, const MarchingCubesDualNode * n7, VolumeSource * source, const DistanceScalerBase * scaler );
	void create_border_cells( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3,
		                      const MarchingCubesDualNode * n4, const MarchingCubesDualNode * n5, const MarchingCubesDualNode * n6, const MarchingCubesDualNode * n7, VolumeSource * source, const DistanceScalerBase * scaler );
	void add_dual_cell( const VectorInt & c0, const VectorInt & c1, const VectorInt & c2, const VectorInt & c3, 
		                const VectorInt & c4, const VectorInt & c5, const VectorInt & c6, const VectorInt & c7, VolumeSource * source, const DistanceScalerBase * scaler );


	void create_faces_in_dual_grid( VolumeSource * source, const DistanceScalerBase * scaler );

    Vector3d interpolate( const Vector3d & v0, const Vector3d & v1, const Float val0, const Float val1 ) const;

    // For BFS search.
    // 26 Node neighbors.
    //void add_node_neighbors( const MarchingCubesDualNode & node, VolumeSource * source, const DistanceScalerBase * scaler, int & nodes_qty );

    // Create faces.
    void create_faces( const MarchingCubesDualCell & node, int material_index = -1 );

    // Compute value with reusing pre-computed values.
    Float value_at( VolumeSource * source, const VectorInt & vector_int, const Vector3d & at );
    int   cell_material( const MarchingCubesDualCell & cell, VolumeSource * source, const DistanceScalerBase * scaler );
    int   material_at( VolumeSource * source, const VectorInt & vector_int, const Vector3d & at, int * priority );

    // Store face normal for node edge.
    void append_normal( const NodeEdgeInt & edge, const Vector3d & n );

	std::vector<MarchingCubesDualNode *>           _octree_nodes;
	std::vector<MarchingCubesDualCell *>           _octree_dual_cells;

    typedef std::set<MarchingNode>                 MarchingSet;
    typedef std::set<MarchingNode>::iterator       MarchingSetIterator;
    typedef std::set<MarchingNode>::const_iterator MarchingSetConstIterator;

    typedef std::map<VectorInt, Float>                 ValuesMap;
    typedef std::map<VectorInt, Float>::iterator       ValuesMapIterator;
    typedef std::map<VectorInt, Float>::const_iterator ValuesMapConstIterator;

    typedef std::map<VectorInt, MaterialWithPriorityDual>                 MaterialsMap;
    typedef std::map<VectorInt, MaterialWithPriorityDual>::iterator       MaterialsMapIterator;
    typedef std::map<VectorInt, MaterialWithPriorityDual>::const_iterator MaterialsMapConstIterator;

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







