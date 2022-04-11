
#include "data_types.h"
#include "vector3d.h"
//#include "hashfuncs.h"

#include <vector>
#include <set>

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
    VectorInt( const & inst )
    {
        *this = inst;
    }
    VectorInt( int x, int y, int z )
    {
        self.x = x;
        self.y = y;
        self.z = z;
    }

};


class MarchingNode
{
public:
    VectorInt at;
    int       size;
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
            at   = inst.qt;
            size = inst.size;
            for ( int i=0; i<8; i++ )
            {
                vertices[i] = inst.vertices[i];
                alues[i]    = inst.values[i];
            }
        }
        return * this;
    }

    MarchingNode( const MarchingNode & inst )
    {
        *this = inst;
    }

    bool has_surface() const
    {
        int above_qty = 0;
        int below_qty = 0;
        for ( int i=0; i<qty; i++ )
        {
            const Float v = values[i];
            if (v > 0.0)
                above_qty += 1;
            else
                below_qty += 1;

            if ( (above_qty > 0) && (below_qty > 0) )
                return true;
        }

        return false;
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

struct MarchingNodeCompare
{
    bool operator()( const MarchingNode & a, const MarchingNode & b )
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
}



class MarchingCubes
{
public:
    MarchingCubes();
    ~MarchingCubes();

    void set_source_transform( const SE3 & se3 );
    

    void find_subdivision_levels( VolumeSource * source, DistanceScaler * scaler = nullptr );
    bool find_surface( VolumeSource * source, DistanceScaler * scaler, MarchingNode & surface_node );
    bool subdivide_source( VolumeSource * source, DistanceScaler * scaler = nullptr );

    Float node_size( int level ) const;
    Vector3d at( const VectorInt & at_i, DistanceScaler * scaler=nullptr ) const;

    void compute_node_values( MarchingNode & node, VolumeSource * source, DistanceScaler * scaler ) const;
    MarchingNode step_towards_surface( const MarchingNode & node, VolumeSource * source, DistanceScaler * scaler ) const;

    



    Float iso_level;
    Float eps;
    // This is the smallest cube size.
    Float step;
    // Number of scale up levels.
    int  levels_qty;

    // Source transform.
    // Points to probe first are sent through this transformation.
    SE3 source_se3;
private:
    Vector3d interpolate( const Vector3d & v0, const Vector3d & v1, const Float val0, const Float val1 ) const;

    // For BFS search.
    // 26 Node neighbors.
    void add_node_neighbors( const MarchingNode & node, VolumeSource * source, DistanceScaler * scaler );

    // Create faces.
    void create_faces( const MarchingNode & node,  )
    
    typedef MarchingSet         std::set<MarchingNode, MarchingNodeCompare>;
    typedef MarchingSetIterator std::set<MarchingNode, MarchingNodeCompare>::iterator;

    MarchingSet _all_nodes;
    MarchingSet _recently_added_nodes;
    MarchingSet _new_candidates;
};



}







