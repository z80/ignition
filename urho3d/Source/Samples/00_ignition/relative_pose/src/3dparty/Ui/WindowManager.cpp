
#include "Urho3D/Urho3DAll.h"
#include "WindowManager.h"
#include "Events3dparty.h"

#include "SettingsWindow.h"
#include "PauseWindow.h"
#include "QuitConfirmationWindow.h"
#include "PopupMessageWindow.h"
#include "main_menu.h"

/// Construct.
WindowManager::WindowManager(Context* context) :
    Object(context)
{
    SubscribeToEvents();
    RegisterAllFactories();
}

WindowManager::~WindowManager()
{
    _windowList.Clear();
}

void WindowManager::RegisterAllFactories()
{
    // Register all available windows
    context_->RegisterFactory<BaseWindow>();
    context_->RegisterFactory<SettingsWindow>();
    //context_->RegisterFactory<ScoreboardWindow>();
    //context_->RegisterFactory<AchievementsWindow>();
    context_->RegisterFactory<QuitConfirmationWindow>();
    //context_->RegisterFactory<NewGameSettingsWindow>();
    //context_->RegisterFactory<AchievementsWindow>();
    context_->RegisterFactory<PauseWindow>();
    context_->RegisterFactory<PopupMessageWindow>();
    Ign::MainMenu::RegisterObject( context_ );
}

void WindowManager::SubscribeToEvents()
{
    SubscribeToEvent(IgnEvents::E_OPEN_WINDOW, URHO3D_HANDLER(WindowManager, HandleOpenWindow));
    SubscribeToEvent(IgnEvents::E_CLOSE_WINDOW, URHO3D_HANDLER(WindowManager, HandleCloseWindow));
    SubscribeToEvent(IgnEvents::E_CLOSE_ALL_WINDOWS, URHO3D_HANDLER(WindowManager, HandleCloseAllWindows));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(WindowManager, HandleUpdate));
}

void WindowManager::HandleOpenWindow(StringHash eventType, VariantMap& eventData)
{
    using namespace IgnEvents::OpenWindow;
    String windowName = eventData["Name"].GetString();
    for (auto it = _windowList.Begin(); it != _windowList.End(); ++it) {
        if ((*it)->GetType() == StringHash(windowName)) {
            if (!(*it).Refs()) {
                _windowList.Erase(it);
            } else {
                URHO3D_LOGWARNING("Window '" + windowName + "' already opened!");
                BaseWindow* window = (*it)->Cast<BaseWindow>();
                //TODO bring this window to the front

                if (eventData.Contains(P_CLOSE_PREVIOUS)) {
                    if (eventData[P_CLOSE_PREVIOUS].GetBool()) {
                        CloseWindow(windowName);
                    } else {
                        return;
                    }
                } else {
                    return;
                }
            }
        }
    }

    URHO3D_LOGINFO("Opening window: " + windowName);
    SharedPtr<Object> newWindow;
    newWindow = context_->CreateObject(StringHash(windowName));
    if (newWindow) {
        BaseWindow *window = newWindow->Cast<BaseWindow>();
        window->SetData(eventData);
        window->Init();
        _windowList.Push(newWindow);

        _openedWindows.Push(windowName);
    } else {
        URHO3D_LOGERROR("Failed to open window: " + windowName);
    }
}

void WindowManager::HandleCloseWindow(StringHash eventType, VariantMap& eventData)
{
    String windowName = eventData["Name"].GetString();
    _closeQueue.Push(windowName);
}

void WindowManager::HandleCloseAllWindows(StringHash eventType, VariantMap& eventData)
{
    URHO3D_LOGINFO("Closing all windows");
    for (auto it = _openedWindows.Begin(); it != _openedWindows.End(); ++it) {
        _closeQueue.Push((*it));
    }

    _openedWindows.Clear();
}

bool WindowManager::IsWindowOpen( const String & windowName )
{
    for (auto it = _windowList.Begin(); it != _windowList.End(); ++it) {
        if ((*it)->GetType() == StringHash(windowName)) {
            BaseWindow* window = (*it)->Cast<BaseWindow>();
            if ((*it).Refs()) {
                URHO3D_LOGINFO(" WINDOW " + windowName + " IS ACTIVE");
                return true;
            }
        }
    }

    URHO3D_LOGINFO(" WINDOW " + windowName + " NOT ACTIVE");
    return false;
}

void WindowManager::OpenWindow( const String & windowName, bool closePrev )
{
    using namespace IgnEvents::OpenWindow;
    VariantMap & m = GetEventDataMap();
    m[P_NAME]           = windowName;
    m[P_CLOSE_PREVIOUS] = closePrev;
    SendEvent( IgnEvents::E_OPEN_WINDOW, m );
}

void WindowManager::OpenWindow( Context * ctx, const String & windowName, bool closePrev )
{
    WindowManager * wm = ctx->GetSubsystem<WindowManager>();
    wm->OpenWindow( windowName, closePrev );
}

void WindowManager::CloseWindow( Context * ctx, const String & windowName )
{
    WindowManager * wm = ctx->GetSubsystem<WindowManager>();
    wm->CloseWindow( windowName );
}

void WindowManager::CloseWindow( const String & windowName )
{
    using namespace IgnEvents::CloseWindow;
    URHO3D_LOGINFO("Closing window: " + windowName);
    const StringHash nameHash = StringHash(windowName);
    for (auto it = _windowList.Begin(); it != _windowList.End(); ++it)
    {
        if ( (*it)->GetType() == nameHash )
        {
            SharedPtr<Object> obj = *it;
            _windowList.Erase(it);
            VariantMap data;
            data[P_NAME] = windowName;
            SendEvent(IgnEvents::E_WINDOW_CLOSED, data);
            return;
        }
    }
}

void WindowManager::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    if (!_closeQueue.Empty()) {
        for (auto it = _closeQueue.Begin(); it != _closeQueue.End(); ++it) {
            CloseWindow((*it));
        }
        _closeQueue.Clear();
    }
}
