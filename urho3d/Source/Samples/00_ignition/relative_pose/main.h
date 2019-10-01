

#pragma once

#include "Sample.h"
#include "item_node.h"

namespace Urho3D
{

class Node;
class Scene;

}

using namespace Ign;

/// Static 3D scene example.
/// This sample demonstrates:
///     - Creating a 3D scene with static content
///     - Displaying the scene using the Renderer subsystem
///     - Handling keyboard and mouse input to move a freelook camera
class Main : public Sample
{
    URHO3D_OBJECT(Main, Sample);

public:
    /// Construct.
    explicit Main(Context* context);

    /// Setup after engine initialization and before running the main loop.
    void Start() override;

private:
    /// Construct the scene content.
    void CreateScene();
    /// Construct an instruction text to the UI.
    void CreateInstructions();
    /// Set up a viewport for displaying the scene.
    void SetupViewport();
    /// Read input and moves the camera.
    void MoveCamera(float timeStep);
    /// Subscribe to application-wide logic update events.
    void SubscribeToEvents();
    /// Handle the logic update event.
    void HandleUpdate( StringHash eventType, VariantMap & eventData );

    ItemNode * nodeA, * nodeB, * nodeC, * nodeD;
};
