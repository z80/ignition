
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
    bool inside;
    FaceId face_id;
    Float  depth;
};
struct BoxFace
{
    FaceId id;
    // Outwards normal.
    Vector3d n;
    // Face center.
    Vector3d center;
    // Two ortogonal vectors along face dimensions.
    Vector3d x, y;
    // Appropriate half sizes.
    Float sz_x, sz_y;
    bool intersects( const BoxVertex & a, const BoxVertex & b, Vector3d & at, Vector3d & depth ) const;
};

struct Box
{
    BoxFace faces[6];
    BoxVertex verts[8];

    void init( const Vector3d sz2 );
    void apply( const Pose & pose );
    bool inside( BoxVertex & v ) const;

};
 
}





