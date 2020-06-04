
#ifndef __OTREE_2_H_
#define __OTREE_2_H_

#include <vector>
#include <map>


namespace KEYPOINT_BA
{
    class ConfigReader;
}

namespace KEYPOINT_BA
{


typedef Eigen::Matrix<Float, 3, 3> EMatrix33;
typedef Eigen::Matrix<Float, 3, 1> EVector3;
typedef Eigen::Matrix<Float, 4, 4> EMatrix44;

class OTree2;
class Camera;

class ONode2
{
public:
    ONode2();
    ~ONode2();
    ONode2( const ONode2 & inst );
    const ONode2 & operator=( const ONode2 & inst );

    ONode2 & parent();

    bool inside( const Point3d & pt ) const;
    bool hasChildren() const;
    bool subdrive();

    void vertices( Point3d * verts ) const;
    void planes( Plane * planes ) const;
    bool intersectsRay( const Point3d & start, const Point3d & end ) const;

    OTree2 * tree;
    int absIndex;
    int parentAbsIndex;
    int indexInParent;
    int level;

    // Child indices in OTree dictionary.
    int children[8];

    Float   value;
    Float   size2; // Size over 2.
    Point3d center;

    std::vector<int> ptInds;
};

struct PtRef2
{
    int     id;
    Point3d pt3;
};

class OTree2
{
public:
    OTree2( int maxPtsPerNode=25, int maxLvl=30 );
    OTree2( KEYPOINT_BA::ConfigReader * cr );
    ~OTree2();

    OTree2( const OTree2 & inst );
    const OTree2 & operator=( const OTree2 & inst );

    void operator=( const Keypoint3d::Map & pts3 );
    void visiblePts3( const Camera & c, std::vector<int> & ptInds ) const;
    void addNodePts3( const Camera & c, const ONode2 & n, std::vector<int> & ptInds ) const;

    bool parent( const ONode2 & node, ONode2 * & parent );
    void destroyNode( const ONode2 & node );

    int  insertNode( ONode2 & node );
    void updateNode( const ONode2 & node );

    bool nodeInvisible( const Camera & c, const ONode2 & n ) const;

    std::vector<ONode2> nodes;
    std::vector<PtRef2> ptRefs;

    // Maximum subdivision level.
    int maxDepth;
    int maxPtsPerNode;
};






}


#endif





