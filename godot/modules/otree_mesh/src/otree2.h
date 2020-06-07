
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


//typedef Eigen::Matrix<Float, 3, 3> EMatrix33;
//typedef Eigen::Matrix<Float, 3, 1> EVector3;
//typedef Eigen::Matrix<Float, 4, 4> EMatrix44;



struct PtRef2
{
    int     id;
    Vector3 pt3;
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





