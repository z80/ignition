
#include <Urho3D/Urho3DAll.h>
#include "design_panel.h"
#include "MyEvents.h"
#include "Audio/AudioManagerDefs.h"
#include "Global.h"

using namespace Urho3D;

namespace Osp
{

/// Construct.
DesignPanel::DesignPanel(Context* context)
    : BaseWindow(context)
{
    Init();
}

DesignPanel::~DesignPanel()
{
    _newGameButton->Remove();
    _exitWindow->Remove();
    _baseWindow->Remove();
}

void DesignPanel::Init()
{
    Create();

    SubscribeToEvents();
}

void DesignPanel::Create()
{
    auto* localization = GetSubsystem<Localization>();

    _baseWindow = GetSubsystem<UI>()->GetRoot()->CreateChild<Window>();
    _baseWindow->SetStyleAuto();
    _baseWindow->SetAlignment(HA_CENTER, VA_CENTER);
    _baseWindow->SetSize(220, 80);
    _baseWindow->BringToFront();

    _newGameButton = CreateButton(localization->Get("START"), 80, IntVector2(20, 0));
    _newGameButton->SetAlignment(HA_LEFT, VA_CENTER);

    SubscribeToEvent(_newGameButton, E_RELEASED, [&](StringHash eventType, VariantMap& eventData) {
        VariantMap& data = GetEventDataMap();
        data["Name"] = "Loading";
        SendEvent(MyEvents::E_SET_LEVEL, data);
    });

    _exitWindow = CreateButton(localization->Get("EXIT"), 80, IntVector2(-20, 0));
    _exitWindow->SetAlignment(HA_RIGHT, VA_CENTER);
    SubscribeToEvent(_exitWindow, E_RELEASED, [&](StringHash eventType, VariantMap& eventData) {
        VariantMap& data = GetEventDataMap();
        data["Name"] = "DesignPanel";
        SendEvent(MyEvents::E_CLOSE_WINDOW, data);
    });
}

void DesignPanel::SubscribeToEvents()
{
}


Button* DesignPanel::CreateButton(const String& text, int width, IntVector2 position)
{
    auto* cache = GetSubsystem<ResourceCache>();
    auto* font = cache->GetResource<Font>(APPLICATION_FONT);

    auto* button = _baseWindow->CreateChild<Button>();
    button->SetStyleAuto();
    button->SetFixedWidth(width);
    button->SetFixedHeight(30);
    button->SetPosition(position);

    auto* buttonText = button->CreateChild<Text>();
    buttonText->SetFont(font, 12);
    buttonText->SetAlignment(HA_CENTER, VA_CENTER);
    buttonText->SetText(text);

    return button;
}

}



