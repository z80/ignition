
#ifndef __OTREE_2_H_
#define __OTREE_2_H_

#include "core/vector.h"
#include "core/math/face3.h"
#include "scene/3d/mesh_instance.h"
#include "onode2.h"


/**
	This data structure is for determining empty/filled space only.
	Nonempty leaf node means space is filled. Else means it's empty.
**/

class OTree2
{
public:
	OTree2( real_t nodeSz=0.1 );
    ~OTree2();

    OTree2( const OTree2 & inst );
    const OTree2 & operator=( const OTree2 & inst );

	void clear();
    void append( const Transform & t, const Ref<Mesh> mesh );
	void subdivide();
    
    bool parent( const ONode2 & node, ONode2 * & parent );

    int  insertNode( ONode2 & node );
    void updateNode( const ONode2 & node );

    Vector<ONode2> nodes;
    Vector<Face3>  ptRefs;

    // Maximum subdivision level.
	real_t node_sz_;
    int    max_depth_;
};







#endif





