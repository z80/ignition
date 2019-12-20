#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/Font.h>
#include "Achievements.h"
#include "../Global.h"

void SingleAchievement::RegisterObject(Context* context)
{
    context->RegisterFactory<SingleAchievement>();
    URHO3D_ATTRIBUTE("Offset", float, _offset, 1, AM_FILE);
}

SingleAchievement::SingleAchievement(Context* context) :
        Animatable(context)
{
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(SingleAchievement, HandlePostUpdate));

    auto *cache = GetSubsystem<ResourceCache>();
    auto *font = cache->GetResource<Font>(APPLICATION_FONT);

    _baseWindow = GetSubsystem<UI>()->GetRoot()->CreateChild<Window>();
    _baseWindow->SetStyleAuto();
    _baseWindow->SetAlignment(HA_LEFT, VA_BOTTOM);
    _baseWindow->SetSize(300, 100);
    _baseWindow->BringToFront();

    _baseWindow->SetBringToBack(true);

    _sprite = _baseWindow->CreateChild<Sprite>();
    _sprite->SetSize(80, 80);
    _sprite->SetAlignment(HA_LEFT, VA_TOP);
    _sprite->SetPosition(10, 10);

    _title = _baseWindow->CreateChild<Text>();
    _title->SetFont(font, 10);
    _title->SetAlignment(HA_LEFT, VA_CENTER);
    _title->SetPosition(100, 0);
    _title->SetWordwrap(true);
    _title->SetWidth(_baseWindow->GetWidth() - _sprite->GetWidth() - 20);
}

SingleAchievement::~SingleAchievement()
{
    _baseWindow->Remove();
    UnsubscribeFromEvent(E_POSTUPDATE);
}

void SingleAchievement::SetImage(String image)
{
    auto* cache = GetSubsystem<ResourceCache>();
    _sprite->SetTexture(cache->GetResource<Texture2D>(image));
}

void SingleAchievement::SetMessage(String message) {
    _message = "";
    _title->SetText(message);
}

String SingleAchievement::GetMessage()
{
    return _message;
}

void SingleAchievement::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace PostUpdate;

    UpdateAttributeAnimations(eventData[P_TIMESTEP].GetFloat());

    _baseWindow->SetPosition(_offset, -10);
}

void SingleAchievement::OnAttributeAnimationAdded()
{
    if (attributeAnimationInfos_.Size() == 1)
        SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(SingleAchievement, HandlePostUpdate));
}

void SingleAchievement::OnAttributeAnimationRemoved()
{
    if (attributeAnimationInfos_.Empty())
        UnsubscribeFromEvent(E_POSTUPDATE);
}

void SingleAchievement::SetVar(StringHash key, const Variant& value)
{
    vars_[key] = value;
}

const Variant& SingleAchievement::GetVar(const StringHash& key) const
{
    VariantMap::ConstIterator i = vars_.Find(key);
    return i != vars_.End() ? i->second_ : Variant::EMPTY;
}