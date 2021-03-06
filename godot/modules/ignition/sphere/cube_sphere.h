
#ifndef __CUBE_SPHERE_H_
#define __CUBE_SPHERE_H_

#include "core/vector.h"
#include "core/hash_map.h"
#include "cube_edge_hash.h"
#include "cube_vertex.h"
#include "subdivide_source.h"

namespace Ign
{

class SubdivideSource;
class HeightSource;

class CubeSphere
{
public:
    struct Level
    {
        Float sz;
        Float dist;
    };

    Vector<CubeVertex>      verts;
    Vector<CubeQuadNode>    faces;
    HashMap<uint64_t, int>  lookup;

    CubeSphere();
    ~CubeSphere();

    CubeSphere( const CubeSphere & inst );
    const CubeSphere & operator=( const CubeSphere & inst );

    /// Initial hash for all the faces.
    void set_hash( uint64_t hash=0 );

    // Radius in physicsl units.
    void set_r( const Float newR );
    Float r() const;
    // Height in physicsl units.
    void set_h( const Float newH );
    Float h() const;
    
    // Subdivision levels.
    // Units are in real distance units. I.e. not normalized.
    void clear_levels();
    void add_level( Float sz, Float dist );


    void subdivide( SubdivideSource * src );
    void apply_source( HeightSource * src );

    void triangle_list( Vector<CubeVertex> & tris );

    // For selecting collision patches.
    void triangle_list( const Vector<Vector3d> & pts, Float dist, Vector<CubeVertex> & tris );
    void face_list( const Vector<Vector3d> & pts, const Float sz, const Float dist, Vector<int> & faceInds );

    // This is for subdividing. It is used in SubdivideSource.
    void flatten_pts( const Vector<Vector3d> & pts, Vector<Vector3d> & ptsFlat ) const;
private:
    void clear();
    void init();
    void label_mid_points();
    void scale_to_sphere();
    void compute_normals();
    void apply_source_height( HeightSource * src, CubeVertex & v );
    void apply_source_color( HeightSource * src, CubeVertex & v );

    void select_faces( const Vector<Vector3d> & pts, const Float dist, Vector<int> & faceInds );

public:
    // Radius unit sphere is scaled to 
    // when returned.
    Float R_, H_;
    uint64_t hash_;
    Vector<Level> levels_, levelsUnit_;

    Vector<Vector3d> ptsFlat_;
    Vector<int>      faceInds_;

    void sort_levels();
};



}




#endif





