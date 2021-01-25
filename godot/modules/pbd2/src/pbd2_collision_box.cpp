
#include "pbd2_collision_box.h"
#include "pbd2_collision_plane.h"
#include "pbd2_collisions.h"

#include "pbd2_broad_node.h"
#include "pbd2_rigid_body.h"
#include "pbd2_collision_utils.h"

namespace Pbd
{


CollisionBox::CollisionBox()
    : CollisionObject()
{
    size2 = Vector3d( 1.0, 1.0, 1.0 );
    obj_type = ObjectBox;
}

CollisionBox::~CollisionBox()
{
}
    
Float CollisionBox::bounding_radius() const
{
    return size2.Length();
}

bool CollisionBox::inside( const BroadTreeNode * n, Float h ) const
{
    const Pose pose_box = pose_w();
    Pose pose_node;
    pose_node.r = n->center;
    Box box_box;
    box_box.init( get_size2() );
    box_box.apply( pose_box );

    Box box_node;
    box_node.init( Vector3d( n->size2, n->size2, n->size2 ) );
    box_node.apply( pose_node );

    const bool ok = box_node.intersects( box_box );
    return ok;
}

void CollisionBox::intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths )
{
    CollisionObjectType tp = b->type();
    if ( tp == ObjectBox )
    {
        CollisionBox * bs = dynamic_cast<CollisionBox *>( b );
        if ( !bs )
            return;
        collision_box_box( this, bs, ats, depths );
    }
    if ( tp == ObjectPlane )
    {
        CollisionPlane * bs = dynamic_cast<CollisionPlane *>( b );
        if ( !bs )
            return;
        collision_box_plane( this, bs, ats, depths );
    }
}

void CollisionBox::set_size( const Vector3d & sz )
{
    size2 = sz * 0.5;
}

Vector3d CollisionBox::get_size() const
{
    return size2 * 2.0;
}

Vector3d CollisionBox::get_size2() const
{
	return size2;
}



}





