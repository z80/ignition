
#ifndef __PBD2_COLLISION_OBJECT_H_
#define __PBD2_COLLISION_OBJECT_H_

#include "pbd2_collision_object.h"
#include "occupancy_types.h"
#include "scene/3d/mesh_instance.h"


namespace Pbd
{

class CollisionGrid: public CollisionObject
{
public:
    CollisionGrid();
    ~CollisionGrid();

    void set_dimx( int x );
    int get_dimx() const;

    void set_dimy( int y );
    int get_dimy() const;

    void set_dimz( int z );
    int get_dimz() const;

    void clear();
    void append_triangle( const Vector3d & a, Vector3d & b, Vector3d & b );
    void append_mesh( const Transform & t, const Ref<Mesh> & mesh );

    void recompute();

    PoolVector3Array lines_nodes() const;
    PoolVector3Array lines_pts() const;


    // Needed by collision object.
    Float bounding_radius() const override;
    bool inside( const BroadTreeNode * n, Float h ) const override;
    void intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths ) override;

public:
    int dimx_, dimy_, dimz_;
    Cube             cube_;
    Vector<Face>     triangles_;
    Vector<Float>    coeffs_;
    Vector<Vector3d> surf_pts_;

private:
    void clear_coeffs();
    bool intersect_grid( NarrowTree * tree, Vector<Vector3d> & pts, Vector<Vector3d> & depths )
    bool intersect_plane( NarrowTree * tree, Vector<Vector3d> & pts, Vector<Vector3d> & depths )
    bool depth( const Vector3d & r, Float & at, Vector3d & to_surf );
};




}


#endif






