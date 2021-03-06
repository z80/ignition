
#include "Urho3D/Urho3DAll.h"
#include "MouseInput.h"
#include "ControllerInput.h"
#include "Events3dparty.h"
#include "Global3dparty.h"

// To be able to work using laptop 
//#define MOUSE_BTN_ORBIT MOUSEB_MIDDLE
#define MOUSE_BTN_ORBIT MOUSEB_RIGHT

/// Construct.
MouseInput::MouseInput(Context* context) :
    BaseInput(context)
{
    visible_ = true;
    select_  = false;
    SetMinSensitivity(0.1f);
    Init();
}

MouseInput::~MouseInput()
{
}

void MouseInput::Init()
{
    // Subscribe to global events for camera movement
    SubscribeToEvents();
}

void MouseInput::SubscribeToEvents()
{
	SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(MouseInput, HandleKeyDown));
	SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(MouseInput, HandleKeyUp));
	SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(MouseInput, HandleMouseMove));
    SubscribeToEvent(E_MOUSEWHEEL, URHO3D_HANDLER(MouseInput, HandleMouseWheel));
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MouseInput, HandleUpdate));
//    GetSubsystem<Input>()->SetTouchEmulation(true);
}

void MouseInput::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
	using namespace MouseButtonDown;
	const int key = eventData[P_BUTTON].GetInt();

	if (_activeAction > 0 && _timer.GetMSec(false) > 100) {
		auto* controllerInput = GetSubsystem<ControllerInput>();
		controllerInput->SetConfiguredKey(_activeAction, key, "mouse");
		_activeAction = 0;
		return;
	}

    // Do not move if the UI has a focused element (the console)
    UI * ui = GetSubsystem<UI>();
    if ( ui->GetFocusElement() )
        return;

    if ( key == MOUSEB_LEFT )
        select_ = true;
    else if ( key == MOUSE_BTN_ORBIT )
        ui->GetCursor()->SetVisible( false );

	if (_mappedKeyToControl.Contains(key)) {
		auto* controllerInput = GetSubsystem<ControllerInput>();
		controllerInput->SetActionState(_mappedKeyToControl[key], true);
	}
}

void MouseInput::HandleKeyUp(StringHash eventType, VariantMap& eventData)
{
	using namespace MouseButtonUp;
	const int key = eventData[P_BUTTON].GetInt();

	if (_activeAction > 0) {
		return;
	}

    // Check if wheel is pressed.
    const int btns = eventData[P_BUTTONS].GetInt();
    const bool visible = GetMouseVisible();
    if ( !visible_ )
    {
        const bool pressed = btns & MOUSE_BTN_ORBIT;
        if ( !pressed )
            SetMouseVisible( true );
    }

    UI * ui = GetSubsystem<UI>();
    if ( (key == MOUSEB_LEFT) && select_ )
    {
        using namespace IgnEvents::SelectRequest;
        VariantMap & data = GetEventDataMap();

        const IntVector2 pos = ui->GetCursorPosition();
        data[P_X] = pos.x_;
        data[P_Y] = pos.y_;
        SendEvent( IgnEvents::E_SELECT_REQUEST, data );
    }
    else if ( key == MOUSE_BTN_ORBIT )
        ui->GetCursor()->SetVisible( true );

    select_ = false;

    // Do not move if the UI has a focused element (the console)
    if ( GetSubsystem<UI>()->GetFocusElement() )
        return;

	if (_mappedKeyToControl.Contains(key)) {
		auto* controllerInput = GetSubsystem<ControllerInput>();
		controllerInput->SetActionState(_mappedKeyToControl[key], false);
	}
}

void MouseInput::HandleMouseMove(StringHash eventType, VariantMap& eventData)
{
    // Do not move if the UI has a focused element (the console)
    if (GetSubsystem<UI>()->GetFocusElement())
        return;

    using namespace MouseMove;

    // Check if wheel is pressed.
    const int btns = eventData[P_BUTTONS].GetInt();
    const bool pressed = btns & MOUSE_BTN_ORBIT;
    if ( pressed )
    {
        const bool visible = GetMouseVisible();
        if ( visible )
            SetMouseVisible( false );
        select_ = false;
    }
    else
        return;

    float dx = eventData[P_DX].GetInt() * _sensitivityX;
    float dy = eventData[P_DY].GetInt() * _sensitivityY;
    if (_invertX) {
        dx *= -1.0f;
    }
    if (_invertY) {
        dy *= -1.0f;
    }
    ControllerInput* controllerInput = GetSubsystem<ControllerInput>();
    controllerInput->UpdateYaw(dx);
    controllerInput->UpdatePitch(dy);
}

void MouseInput::HandleMouseWheel( StringHash eventType, VariantMap & eventData )
{
    // Do not move if the UI has a focused element (the console)
    if (GetSubsystem<UI>()->GetFocusElement())
        return;

    using namespace MouseWheel;

    const int dx    = eventData[P_WHEEL].GetInt();
    const int btns  = eventData[P_BUTTONS].GetInt();
    const int quals = eventData[P_QUALIFIERS].GetInt();

    ControllerInput * controllerInput = GetSubsystem<ControllerInput>();
    controllerInput->UpdateZoom( -dx );
}

bool MouseInput::GetMouseContained() const
{
    Input * input = GetSubsystem<Input>();
    const MouseMode m = input->GetMouseMode();
    const bool contained = (m != MM_ABSOLUTE);
    return contained;
}

void MouseInput::SetMouseContained( bool en )
{
    Input * input = GetSubsystem<Input>();
    input->SetMouseMode( en ? MM_RELATIVE : MM_ABSOLUTE );
}

bool MouseInput::GetMouseVisible() const
{
    return visible_;
}

void MouseInput::SetMouseVisible( bool en )
{
    Input * input = GetSubsystem<Input>();
    input->SetMouseVisible( en );
    visible_ = en;
}

String MouseInput::GetActionKeyName(int action)
{
	if (_mappedControlToKey.Contains(action)) {
		auto* input = GetSubsystem<Input>();
		int key = _mappedControlToKey[action];
		if (key == MOUSEB_LEFT) {
			return "MOUSEB_LEFT";
		}
		if (key == MOUSEB_MIDDLE) {
			return "MOUSEB_MIDDLE";
		}
		if (key == MOUSEB_RIGHT) {
			return "MOUSEB_RIGHT";
		}
	}

	return String::EMPTY;
}

void MouseInput::LoadConfig()
{
    _sensitivityX = GetSubsystem<ConfigManager>()->GetFloat("mouse", "Sensitivity", 1.0f);
    _sensitivityY = GetSubsystem<ConfigManager>()->GetFloat("mouse", "Sensitivity", 1.0f);
    _invertX = GetSubsystem<ConfigManager>()->GetBool("mouse", "InvertX", false);
    _invertY = GetSubsystem<ConfigManager>()->GetBool("mouse", "InvertY", false);
}

void MouseInput::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	auto input = GetSubsystem<Input>();
	ControllerInput* controllerInput = GetSubsystem<ControllerInput>();
	for (unsigned i = 0; i < input->GetNumTouches(); ++i) {
		TouchState* state = input->GetTouch(i);
		if (!state->touchedElement_)    // Touch on empty space
		{
			if (state->delta_.x_ || state->delta_.y_)
			{
				float yaw = _sensitivityX * state->delta_.x_;
				float pitch = _sensitivityY * state->delta_.y_;
				controllerInput->UpdateYaw(yaw);
				controllerInput->UpdatePitch(pitch);
			}
		}
	}
}
