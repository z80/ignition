
#ifndef __STATIC_MESH_H_
#define __STATIC_MESH_H_

#include "ref_frame.h"

using namespace Urho3D;

namespace Ign
{

class StaticMesh: public RefFrame
{
    URHO3D_OBJECT( StaticMesh, RefFrame )
public:
    static void RegisterObject( Context * context );

    StaticMesh( Context * ctx );
    virtual ~StaticMesh();

    virtual void refStateChanged() override;

protected:
    virtual void OnNodeSet( Node * node );
    /// Handle scene being assigned. This may happen several times
    //during the component's lifetime. Scene-wide subsystems and events
    //are subscribed to here.
    virtual void OnSceneSet( Scene * scene );
    /// Handle scene node transform dirtied.
    virtual void OnMarkedDirty( Node * node );
    /// Handle scene node enabled status changing.
    virtual void OnNodeSetEnabled( Node * node );

private:
    SharedPtr<Node> node_;

};

}


#endif





