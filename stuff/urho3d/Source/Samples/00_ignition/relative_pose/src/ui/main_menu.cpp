
#include "main_menu.h"
#include "environment.h"
#include "WindowManager.h"

namespace Ign
{

void MainMenu::RegisterObject( Context * context )
{
    context->RegisterFactory<MainMenu>();
}

MainMenu::MainMenu( Context * context )
  : BaseWindow( context )
{
}

MainMenu::~MainMenu()
{
    Dispose();
}

void MainMenu::Init()
{
    Create();
    SubscribeToEvents();
}

void MainMenu::Create()
{

}

void MainMenu::Dispose()
{
    if ( mainWnd_ )
        mainWnd_->Remove();
}


void MainMenu::SetVisible( bool en )
{
    mainWnd_->SetVisible( en );
}

void MainMenu::SubscribeToEvents()
{
    auto * cache = GetSubsystem<ResourceCache>();
    auto * ui = GetSubsystem<UI>();

    // Set up global UI style into the root UI element
    auto * style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    ui->GetRoot()->SetDefaultStyle(style);

    // Create a Cursor UI element because we want to be able to hide and show it at will. When hidden, the mouse cursor will
    // control the camera, and when visible, it will interact with the UI
    SharedPtr<Cursor> cursor(new Cursor(context_));
    cursor->SetStyleAuto();
    ui->SetCursor(cursor);
    // Set starting position of the cursor at the rendering window center
    auto * graphics = GetSubsystem<Graphics>();
    cursor->SetPosition(graphics->GetWidth() / 2, graphics->GetHeight() / 2);

    // Load UI content prepared in the editor and add to the UI hierarchy
    mainWnd_ = ui->LoadLayout( cache->GetResource<XMLFile>( "Ign/UI/MainMenu.xml") );
    ui->GetRoot()->AddChild( mainWnd_ );

    // Subscribe to button actions (toggle scene lights when pressed then released)
    auto * button = mainWnd_->GetChildStaticCast<Button>("NewGame", true);
    if ( button )
        SubscribeToEvent( button, E_RELEASED, URHO3D_HANDLER( MainMenu, HandleNewGame ) );

    button = mainWnd_->GetChildStaticCast<Button>( "Connect", true );
    if ( button )
        SubscribeToEvent( button, E_RELEASED, URHO3D_HANDLER( MainMenu, HandleConnect ) );

    button = mainWnd_->GetChildStaticCast<Button>( "Settings", true );
    if ( button )
        SubscribeToEvent( button, E_RELEASED, URHO3D_HANDLER( MainMenu, HandleSettings ) );

    button = mainWnd_->GetChildStaticCast<Button>( "Exit", true );
    if ( button )
        SubscribeToEvent( button, E_RELEASED, URHO3D_HANDLER( MainMenu, HandleExit ) );
}

void MainMenu::HandleNewGame( StringHash event, VariantMap & args )
{
    RefCounted * rc = GetGlobalVar( StringHash( "Environment" ) ).GetPtr();
    Environment * e = dynamic_cast<Environment *>( rc );

    if ( !e )
        return;

    e->StartServer();

    SetVisible( false );
}

void MainMenu::HandleConnect( StringHash event, VariantMap & args )
{
    RefCounted * rc = GetGlobalVar( StringHash( "Environment" ) ).GetPtr();
    Environment * e = dynamic_cast<Environment *>( rc );
    if ( !e )
        return;

    ClientDesc c;
    c.login_ = "login";
    c.password_ = "password";
    e->Connect( c );

    //SetVisible( false );
    WindowManager::CloseWindow( context_, "MainMenu" );
}

void MainMenu::HandleSettings( StringHash event, VariantMap & args )
{
    WindowManager::OpenWindow( context_, "SettingsWindow", true );
    //SetVisible( false );
}

void MainMenu::HandleExit( StringHash event, VariantMap & args )
{
    Engine * e = GetSubsystem<Engine>();
    e->Exit();
}


}





