
#ifndef __INFINITE_PLAIN_H_
#define __INFINITE_PLAIN_H_

#include "Urho3D/Urho3DAll.h"
#include "ref_frame.h"

namespace Ign
{

class InfinitePlane: public RefFrame
{
    URHO3D_OBJECT( InfinitePlane, RefFrame )
public:
    static void RegisterComponent( Context * context );

    InfinitePlane( Context * context );
    ~InfinitePlane();

    void refStateChanged() override;
    void poseChanged() override;

protected:
    /// Handle scene being assigned. This may happen several times
    //during the component's lifetime. Scene-wide subsystems and events
    //are subscribed to here.
    void OnSceneSet( Scene * scene ) override;

private:
    SharedPtr<Node> node_;
    Vector<SharedPtr<Node> > nodes_;
    static const int QTY;
    static const Float STEP;
};

}


#endif





