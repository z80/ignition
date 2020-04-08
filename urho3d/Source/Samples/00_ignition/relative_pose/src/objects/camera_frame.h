
#ifndef __CAMERA_FRAME_H_
#define __CAMERA_FRAME_H_

#include "ref_frame.h"

namespace Ign
{

class CameraFrame: public RefFrame
{
    URHO3D_OBJECT( CameraFrame, RefFrame )
public:
    enum CameraMode { TGeocentric=0, TFree, TFirstPerson };

    static void RegisterComponent( Context * context );

    void CheckAttributes();

    CameraFrame( Context * context );
    ~CameraFrame();

    void Select( RefFrame * rf );
    void Unselect( RefFrame * rf );
    void Focus( RefFrame * rf );

    void ApplyControls( const Controls & ctrl, Float dt ) override;

    void setCameraMode( CameraMode mode );

    RefFrame * CameraOrigin();

protected:
    void OnSceneSet( Scene * scene ) override;

    void initGeocentric();
    void adjustGeocentric();
    static RefFrame * orbitingFrame( RefFrame * rf );
public:
    // Getters and setters for attributestransfered over network.
    void SetSelectedId( int id );
    int GetSelectedId() const;
    void SetFocusedId( int id );
    int GetFocusedId() const;
    // Makes sure that Ids match object pointers (just 
    // in case if id is set before appropriate obsect is created).
    void EnsureCorrectObjects();

    void refStateChanged() override;
    void assignCameraNode();
    /// Camera node.
    SharedPtr<Node> node_;

    /// Camera parameters.
    Float yaw_, pitch_;
    Float dist_;


    CameraMode camera_mode_;

    // Geocentric mode numbers.
    // If it was initialized already.
    bool     geocentric_initialized_;
    // Previous "up" vector.
    Vector3d geocentric_last_up_;

    Quaterniond surfQ_;


    /// Selection parameters.
    int selected_frame_id_;
    SharedPtr<RefFrame> selected_frame_;

    int focused_frame_id_;
    SharedPtr<RefFrame> focused_frame_;

    // This one is for adjusting the distance.
    static const Float alpha_;
};


}


#endif

