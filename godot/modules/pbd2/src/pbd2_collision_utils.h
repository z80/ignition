
#ifndef __COLLISION_UTILS_H_
#define __COLLISION_UTILS_H_


#include "data_types.h"
#include "vector3d.h"
#include "quaterniond.h"
#include "pbd2_pose.h"

using namespace Ign;

namespace Pbd
{

enum FaceId { BACK=0, RIGHT=1, FORWARD=2, LEFT=3, DOWN=4, UP=5 };
struct BoxVertex
{
    Vector3d v;
    bool     inside;
    int      face_id;
    Float    depth;
};
struct BoxFace
{
    // Outwards normal.
    Vector3d n;
    // Face center.
    Vector3d center;
    // Two ortogonal vectors along face dimensions.
    Vector3d x, y;
    // Appropriate half sizes.
    Float sz_x, sz_y;
	// Edge indices in the box.
	int edge_inds[4];

	// Cross line with face, return line parameter and intersection point.
	bool intersects( const BoxVertex & a, const BoxVertex & b, Float & t_at, Vector3d & v_at ) const;

	// Returns if there is an intersection.
	// Returns through arguments where the intersection occured,
	// the depth is determined as the smallest common perpendicular with all face edges.
    //bool intersects( const BoxVertex & a, const BoxVertex & b, Vector3d & at, Vector3d & depth ) const;
};

struct Box
{
    BoxFace faces[6];
    BoxVertex verts[8];

    void init( const Vector3d & sz2 );
    void apply( const Pose & pose );
    bool inside( BoxVertex & v ) const;
    bool inside_const( const BoxVertex & v ) const;
	bool inside_const( const Vector3d & v ) const;
	bool intersects( const Box & b ) const;

	void collisions_with( const Box & box_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths );

	// Auxilary containers for computing collision points.
	Vector<int>      verts_a_inside_b;
	Vector<Vector3d> edges_a_crossing_b;
	Vector<int>      verts_b_inside_a;
	Vector<Vector3d> edges_b_crossing_a;


	static const int vert_inds[12][2];

	static int all_box_intersections( const Box & box_a, const Box & box_b,
		Vector<int> & verts_a_inside_b, 
		Vector<Vector3d> & edges_a_crossing_b,
		Vector<int> & verts_b_inside_a, 
		Vector<Vector3d> & edges_b_crossing_a );

	static int box_nearest_face( const Box & box, 
		const Vector<Vector3d> & edges_a_crossing_b,
		const Vector<Vector3d> & edges_b_crossing_a );
};

bool common_perp( const Vector3d & a1, const Vector3d & a2, const Vector3d & b1, const Vector3d & b2,
	              Vector3d & at_a, Vector3d & at_b );


}





#endif


