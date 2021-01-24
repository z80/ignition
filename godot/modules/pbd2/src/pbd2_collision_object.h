
#ifndef __PBD2_COLLISION_OBJECT_H_
#define __PBD2_COLLISION_OBJECT_H_

#include "data_types.h"
#include "pbd2_pose.h"
#include "pbd2_contact_point_bb.h"


namespace Pbd
{

class RigidBody;
class BroadTreeNode;

class CollisionObject
{
public:
    enum CollisionObjectType 
    {
        WrongType = 0, 
        Point     = 1, 
        Sphere    = 2, 
        Box       = 3, 
        Plane     = 4, 
        Heightmap = 5
    };

    CollisionObject();
    virtual ~CollisionObject();

    CollisionObjectType type() const;
    
    // It doesn't just return the origin of the ref. frame as 
    // the volume of the collision object might be displaced with respect to it.
    // For example in Octree object Octree center is determined after analysing all the vertices.
    // And it doesn't only depend of "pose" field.
    virtual Vector3d center() const;
    // Here it is a timestep.
    Float    size2( Float h ) const;
    // This should be defined by implementations.
    virtual Float bounding_radius() const;


    // Instead bounding sphere thing to determine if the object is inside broad tree node, 
    // let the object decide it.
    virtual bool inside( BroadTreeNode * n, Float h ) const;



    // Intersecting this object with another collision object.
    virtual void intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths );

    // Computes and returns world pose.
    Pose pose_w() const;

    CollisionObjectType obj_type;
    // RigidBody it is a child of.
    RigidBody * rigid_body;
    // Pose within its rigid body.
    // Global pose is "rigid_body->pose * pose".
    Pose pose;
    // Safety parameter.
    Float k;
    // All contacts with all other bodies.
    Vector<ContactPointBb> contacts;
};


}


#endif






