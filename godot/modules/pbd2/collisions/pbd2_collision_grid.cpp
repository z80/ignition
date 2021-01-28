
#include "pbd2_collision_grid.h"

namespace Pbd
{

CollisionGrid::CollisionGrid()
    : CollisionObject();
{
    dimx_ = 2;
    dimy_ = 2;
    dimz_ = 2;
}

CollisionGrid::~CollisionGrid()
{
}

void CollisionGrid::set_dimx( int x )
{
    if ( x > 0 )
        dimx_ = x;
    else
        dimx_ = 1;
    clear_coeffs();
}

int CollisionGrid::get_dimx() const
{
    return dimx_;
}

void CollisionGrid::set_dimy( int y )
{
    if ( y > 0 )
        dimy_ = y;
    else
        dimy_ = 1;
    clear_coeffs();
}

int CollisionGrid::get_dimy() const
{
    return dimy_;
}

void CollisionGrid::set_dimz( int z )
{
    if ( z > 0 )
        dimz_ = z;
    else
        dimz_ = 1;
    clear_coeffs();
}

int CollisionGrid::get_dimz() const
{
    return dimz_;
}

void CollisionGrid::clear()
{
    triangles_.clear();
    clear_coeffs();
}
void CollisionGrid::append_triangle( const Vector3d & a, Vector3d & b, Vector3d & c )
{
    Face f;
    f.init( a, b, c );
    faces_.push_back( f );
}

void CollisionGrid::append_mesh( const Transform & t, const Ref<Mesh> & mesh )
{
}

void CollisionGrid::recompute()
{
}

PoolVector3Array CollisionGrid::lines_nodes() const
{
}

PoolVector3Array CollisionGrid::lines_pts() const
{
}

Float CollisionGrid::bounding_radius() const
{
}

bool CollisionGrid::inside( const BroadTreeNode * n, Float h ) const
{
}

void CollisionGrid::intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
}


void CollisionGrid::clear_coeffs()
{
    coeffs_.clear();
    surf_pts_.clear();
}








}







