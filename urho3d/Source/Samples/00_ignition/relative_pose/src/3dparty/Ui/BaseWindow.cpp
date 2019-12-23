
#include "Urho3D/Urho3DAll.h"
#include "BaseWindow.h"
#include "Events3dparty.h"

/// Construct.
BaseWindow::BaseWindow(Context* context):
    Object(context)
{
    Init();
}

BaseWindow::~BaseWindow()
{
    Dispose();
    if (_overlay) {
        _overlay->Remove();
    }
}

Sprite* BaseWindow::CreateOverlay()
{
    if (_overlay) {
        _overlay->Remove();
    }
    auto* cache = GetSubsystem<ResourceCache>();

    _overlay = GetSubsystem<UI>()->GetRoot()->CreateChild<Sprite>();
    _overlay->SetEnabled(true);
    _overlay->SetPosition(0, 0);
    _overlay->SetTexture(cache->GetResource<Texture2D>("Textures/Transparent.png"));
    _overlay->SetFixedWidth(GetSubsystem<Graphics>()->GetWidth() / GetSubsystem<UI>()->GetScale());
    _overlay->SetFixedHeight(GetSubsystem<Graphics>()->GetHeight() / GetSubsystem<UI>()->GetScale());
    _overlay->SetBlendMode(BlendMode::BLEND_ALPHA);

    // In cases where resolution is changed, we have to resize the overlay to match the new resolution
    SubscribeToEvent(IgnEvents::E_VIDEO_SETTINGS_CHANGED, [&](StringHash eventType, VariantMap& eventData) {
        _overlay->SetFixedWidth(GetSubsystem<Graphics>()->GetWidth() / GetSubsystem<UI>()->GetScale());
        _overlay->SetFixedHeight(GetSubsystem<Graphics>()->GetHeight() / GetSubsystem<UI>()->GetScale());
    });

    return _overlay;
}

void BaseWindow::SetData(VariantMap data)
{
    _data = data;
}
