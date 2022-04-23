
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

namespace Ign
{

class VolumeSource;
class DistanceScaler;

class VectorInt
{
public:
    int x, y, z;
    VectorInt()
    {
        x = y = z = 0;
    }
    const VectorInt & operator=( const VectorInt & inst )
    {
        if ( this != &inst )
        {
            x = inst.x;
            y = inst.y;
            z = inst.z;
        }
        return *this;
    }
	bool operator==( const VectorInt & inst ) const
	{
		if ( this == &inst )
			return true;
		if (x != inst.x)
			return false;
		if (y != inst.y)
			return false;
		if (z != inst.z)
			return false;
		return true;
	}
	bool operator!=( const VectorInt & inst ) const
	{
		const bool ret = !(*this == inst);
		return ret;
	}
	bool operator<( const VectorInt & inst ) const
	{
		if ( x < inst.x )
			return true;
		else if ( x > inst.x )
			return false;

		if ( y < inst.y )
			return true;
		else if ( y > inst.y )
			return false;

		if ( z < inst.z )
			return true;
		else if ( z > inst.z )
			return false;

		return false;
	}
    VectorInt( const VectorInt & inst )
    {
        *this = inst;
    }
    VectorInt( int x, int y, int z )
    {
        this->x = x;
		this->y = y;
		this->z = z;
    }

};


class MarchingNode
{
public:
    VectorInt at;
    int       size;
	VectorInt vertices_int[8];
    Vector3d  vertices[8];
    Float     values[8];


    MarchingNode()
    {
        at   = VectorInt();
        size = 1;
        for ( int i=0; i<8; i++ )
            values[i] = 0.0;
    }

    ~MarchingNode()
    {}

    const MarchingNode & operator=( const MarchingNode & inst )
    {
        if ( this != &inst )
        {
            at   = inst.at;
            size = inst.size;
            for ( int i=0; i<8; i++ )
            {
				vertices_int[i] = inst.vertices_int[i];
                vertices[i]     = inst.vertices[i];
                values[i]       = inst.values[i];
            }
        }
        return * this;
    }

	bool operator==( const MarchingNode & inst ) const
	{
		if (this == &inst)
			return true;
		if (at != inst.at)
			return false;
		if (size != inst.size)
			return false;
		return true;
	}

	bool operator<( const MarchingNode & inst ) const
	{
		if ( at.x < inst.at.x )
			return true;
		else if ( at.x > inst.at.x )
			return false;

		if ( at.y < inst.at.y )
			return true;
		else if ( at.y > inst.at.y )
			return false;

		if ( at.z < inst.at.z )
			return true;
		else if ( at.z > inst.at.z )
			return false;

		if ( size < inst.size )
			return true;
		else if ( size > inst.size )
			return false;

		return false;
	}

    MarchingNode( const MarchingNode & inst )
    {
        *this = inst;
    }

    bool has_surface( const Float iso_level ) const
    {
        int above_qty = 0;
        int below_qty = 0;
        for ( int i=0; i<8; i++ )
        {
            const Float v = values[i];
            if (v > iso_level)
                above_qty += 1;
            else if (v < iso_level)
                below_qty += 1;

            if ( (above_qty > 0) && (below_qty > 0) )
                return true;
        }

        return false;
    }
};


class NodeEdgeInt
{
public:
	VectorInt a, b;

	NodeEdgeInt()
	{}
	~NodeEdgeInt()
	{}
	const NodeEdgeInt & operator=( const NodeEdgeInt & inst )
	{
		if ( this != &inst )
		{
			a = inst.a;
			b = inst.b;
		}
		return *this;
	}
	NodeEdgeInt( const NodeEdgeInt & inst )
	{
		*this = inst;
	}

	bool operator<( const NodeEdgeInt & other )
	{
		if ( a.x < other.a.x )
			return true;
		else if ( a.x > other.a.x )
			return false;

		if ( a.y < other.a.y )
			return true;
		else if ( a.y > other.a.y )
			return false;

		if ( a.z < other.a.z )
			return true;
		else if ( a.z > other.a.z )
			return false;


		if ( b.x < other.b.x )
			return true;
		else if ( b.x > other.b.x )
			return false;

		if ( b.y < other.b.y )
			return true;
		else if ( b.y > other.b.y )
			return false;

		if ( b.z < other.b.z )
			return true;
		else if ( b.z > other.b.z )
			return false;

		return false;
	}
};


class NormalsAndQty
{
public:
	Vector3d norms;
	int      qty;

	NormalsAndQty()
	{
		norms = Vector3d::ZERO;
		qty   = 0;
	}
	~NormalsAndQty()
	{}
	const NormalsAndQty & operator=( const NormalsAndQty & inst )
	{
		if ( this != &inst )
		{
			norms = inst.norms;
			qty   = inst.qty;
		}
		return *this;
	}
	NormalsAndQty( const NormalsAndQty & inst )
	{
		*this = inst;
	}
};




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
	const Vector<Face3> & faces() const;

    Float    node_size( int level ) const;
    Vector3d at( const VectorInt & at_i, const DistanceScaler * scaler=nullptr ) const;


    



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
    void create_faces( const MarchingNode & node );

	// Compute value with reusing pre-computed values.
	Float value_at( VolumeSource * source, const VectorInt & vector_int, const Vector3d & at );
    
    typedef std::set<MarchingNode>                 MarchingSet;
    typedef std::set<MarchingNode>::iterator       MarchingSetIterator;
	typedef std::set<MarchingNode>::const_iterator MarchingSetConstIterator;

	typedef std::map<VectorInt, Float>                 ValuesMap;
	typedef std::map<VectorInt, Float>::iterator       ValuesMapIterator;
	typedef std::map<VectorInt, Float>::const_iterator ValuesMapConstIterator;

	typedef std::map<NodeEdgeInt, NormalsAndQty>                 NormalsMap;
	typedef std::map<NodeEdgeInt, NormalsAndQty>::iterator       NormalsMapIterator;
	typedef std::map<NodeEdgeInt, NormalsAndQty>::const_iterator NormalsMapConstIterator;


    MarchingSet _all_nodes;
    MarchingSet _recently_added_nodes;
    MarchingSet _new_candidates;

	Vector<Face3> _all_faces;

	// In order to not compute values a few times.
	ValuesMap     _values_map;

	// In order to compute normals.
	NormalsMap    _normals_map;
};



}








#endif







