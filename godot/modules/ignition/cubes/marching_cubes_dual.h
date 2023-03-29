
#ifndef __MARCHING_CUBES_DUAL_H_
#define __MARCHING_CUBES_DUAL_H_




#include "data_types.h"
#include "vector3d.h"
//#include "hashfuncs.h"
#include "core/vector.h"
#include "core/math/face3.h"
#include "core/math/vector2.h"

#include "se3.h"


#include <vector>
#include <set>
#include <map>

#include "cube_types.h"

namespace Ign
{

class VolumeSource;
class MaterialSource;

class MarchingCubesDualNode;
class MarchingCubesDualCell;

class VolumeNodeSizeStrategy;

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

	DualCellOctreeNodePair( const MarchingCubesDualCell * c, const MarchingCubesDualNode * n )
	{
		cell = const_cast<MarchingCubesDualCell *>( c );
		node = const_cast<MarchingCubesDualNode *>( n );
	}
};

bool operator<( const DualCellOctreeNodePair & a, const DualCellOctreeNodePair & b );



class OctreeNodeFaceIndexPair
{
public:
	MarchingCubesDualNode * node;
	int                     face_index;

	OctreeNodeFaceIndexPair()
	{
		node       = nullptr;
		face_index = -1;
	}

	~OctreeNodeFaceIndexPair()
	{
	}

	OctreeNodeFaceIndexPair( const MarchingCubesDualNode * n, int face_ind )
	{
		node       = const_cast<MarchingCubesDualNode *>( n );
		face_index = face_ind;
	}

	OctreeNodeFaceIndexPair( const OctreeNodeFaceIndexPair & inst )
	{
		*this = inst;
	}

	const OctreeNodeFaceIndexPair & operator=( const OctreeNodeFaceIndexPair & inst )
	{
		if ( this != &inst )
		{
			node       = inst.node;
			face_index = inst.face_index;
		}

		return *this;
	}
};

bool operator<( const OctreeNodeFaceIndexPair & a, const OctreeNodeFaceIndexPair & b );








class MarchingCubesDual
{
public:
    MarchingCubesDual();
    ~MarchingCubesDual();

	void set_min_step( Float step );
	Float get_min_step() const;
	Float init_min_step( VolumeSource * source );

	MarchingCubesDualNode create_bounding_node( const Vector3d & contains_pt, Float desired_size ) const;

    bool subdivide_source( VolumeSource * source, VolumeNodeSizeStrategy * strategy = nullptr );
	bool subdivide_source( const MarchingCubesDualNode & bounding_node, VolumeSource * source, VolumeNodeSizeStrategy * strategy = nullptr );


	const std::vector<int> & query_close_nodes( const Vector3d & at_in_source, Float dist, Float max_size );
	Vector3d center_direction( const SE3 & source_se3, const Vector3d & at ) const;
	MarchingCubesDualNode * get_tree_node( int ind );
	bool point_inside_node( int node_ind, const Vector3d & at_in_source );

	bool intersect_with_segment( MarchingCubesDualNode * node, const Vector3d & start, const Vector3d & end, Vector3d & at, Vector3d & norm );
	bool intersect_with_ray( MarchingCubesDualNode * node, const Vector3d & start, const Vector3d & dir, Vector3d & at, Vector3d & norm );

	// These two are called by nodes.
	void set_split_precision( Float rel_diff );
	Float get_split_precision() const;
	bool should_split( MarchingCubesDualNode * node, int level, VolumeSource * source, VolumeNodeSizeStrategy * strategy );
	MarchingCubesDualNode * create_node();
	MarchingCubesDualCell * create_dual_cell();

	SE3 se3_in_point( const Vector3d & at ) const;
	SE3 asset_se3( const SE3 & src_se3, const SE3 & asset_at ) const;


    const std::set<int>        & materials() const;
    const std::vector<Vector3> & vertices( const SE3 & src_se3, int material_ind, Float scale );
    const std::vector<Vector3> & normals( const SE3 & src_se3, int material_ind );
    const std::vector<real_t>  & tangents( const SE3 & src_se3, int material_ind );
	void uvs( int material_ind, const std::vector<Vector2> * & uvs, const std::vector<Vector2> * & uv2s );

	// For asynchronous computations.
	int precompute_scaled_values( const SE3 & src_se3, int material_ind, Float scale = -1.0, const Vector3d & world_pos_bias = Vector3d() );
	const std::vector<Vector3> & vertices() const;
	const std::vector<Vector3> & normals() const;
	const std::vector<real_t>  & tangents() const;
	const std::vector<Vector2> & uvs() const;
	const std::vector<Vector2> & uv2s() const;



    const std::vector<Vector3> & collision_faces( const SE3 & src_se3, const Float dist );

    Float    node_size_min( const MarchingCubesDualNode * node ) const;
	Float    node_size_max( const MarchingCubesDualNode * node ) const;
    Vector3d at_in_source( const VectorInt & at_i, VolumeNodeSizeStrategy * strategy=nullptr ) const;

	VectorInt vector_int( const Vector3d & at ) const;
	Integer   closest_int_size( Float sz ) const;
	VectorInt node_int_origin( const Vector3d & at, Integer node_size_int ) const;

	Float node_size( const MarchingCubesDualNode * node ) const;

    
	// Debug methods.
	int  get_nodes_qty() const;
	void get_node( int node_ind, Vector3d * corners ) const;
	int  get_node_parent( int node_ind ) const;

	int  get_dual_cells_qty() const;
	void get_dual_cell( int cell_ind, Vector3d * corners ) const;
	


    Float iso_level;
	Float max_diff;
    Float eps;
    // This is the smallest cube size.
    Float step;
    // Max allowed number of nodes.
    int   max_nodes_qty;

public:
	void cleanup_nodes();

    int find_subdivision_levels( Float bounding_radius, VolumeSource * source );
    void compute_node_values( MarchingCubesDualNode & node, VolumeSource * source, VolumeNodeSizeStrategy * strategy );
	void assign_node_indices();


	// Dual grid generation methods.
	void node_proc( const MarchingCubesDualNode * n, VolumeSource * source, VolumeNodeSizeStrategy * strategy );
	void face_proc_xy( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, VolumeSource * source, VolumeNodeSizeStrategy * strategy );
	void face_proc_zy( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, VolumeSource * source, VolumeNodeSizeStrategy * strategy );
	void face_proc_xz( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, VolumeSource * source, VolumeNodeSizeStrategy * strategy );
	void edge_proc_x( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3, VolumeSource * source, VolumeNodeSizeStrategy * strategy );
	void edge_proc_y( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3, VolumeSource * source, VolumeNodeSizeStrategy * strategy );
	void edge_proc_z( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3, VolumeSource * source, VolumeNodeSizeStrategy * strategy );
	void vert_proc( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3,
		            const MarchingCubesDualNode * n4, const MarchingCubesDualNode * n5, const MarchingCubesDualNode * n6, const MarchingCubesDualNode * n7, VolumeSource * source, VolumeNodeSizeStrategy * strategy );
	void create_border_cells( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3,
		                      const MarchingCubesDualNode * n4, const MarchingCubesDualNode * n5, const MarchingCubesDualNode * n6, const MarchingCubesDualNode * n7,
		                      VolumeSource * source, VolumeNodeSizeStrategy * strategy );
	MarchingCubesDualCell * add_dual_cell( const VectorInt & c0, const VectorInt & c1, const VectorInt & c2, const VectorInt & c3, 
		                                   const VectorInt & c4, const VectorInt & c5, const VectorInt & c6, const VectorInt & c7,
									       VolumeSource * source, VolumeNodeSizeStrategy * strategy );


	void create_faces_in_dual_grid( VolumeSource * source );
	void assign_faces_to_octree_nodes();

    Vector3d interpolate( const Vector3d & v0, const Vector3d & v1, const Float val0, const Float val1 ) const;

    // Create faces.
    void create_faces( const MarchingCubesDualCell & node, int material_index = -1 );

    // Compute value with reusing pre-computed values.
    Float value_at( VolumeSource * source, const VectorInt & vector_int, const Vector3d & at );
    int   cell_material( const MarchingCubesDualCell & cell, VolumeSource * source );
    int   material_at( VolumeSource * source, const VectorInt & vector_int, const Vector3d & at, int * priority );

    // Store face normal for node edge.
    void append_normal( const NodeEdgeInt & edge, const Vector3d & n );

	const NodeFace & get_face_by_index( int ind );

	std::vector<MarchingCubesDualNode *>           _octree_nodes;
	std::vector<int>	                           _octree_node_indices_result;
	std::vector<MarchingCubesDualCell *>           _octree_dual_cells;
	std::vector<DualCellOctreeNodePair>            _dual_cell_octree_node_pairs;
	std::vector<OctreeNodeFaceIndexPair>           _octree_node_face_indices;

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

    std::vector<NodeFace> _all_faces;

    // For returning results.
    std::vector<Vector3>  _ret_verts;
    std::vector<Vector3>  _ret_norms;
    std::vector<real_t>   _ret_tangs;
	std::vector<Vector2>  _ret_uvs, _ret_uv2s;

    // In order to not compute values a few times.
    ValuesMap     _values_map;
    MaterialsMap  _materials_map;

    // In order to compute normals.
    NormalsMap    _normals_map;

    // Materials set in order to know how many materials in total actually are involved.
    MaterialsSet _materials_set;

	// Face indisce set. Used to query all faces.
    MaterialsSet _face_indices_set;
    std::vector<Vector3> _faces_ret;
};



}








#endif







