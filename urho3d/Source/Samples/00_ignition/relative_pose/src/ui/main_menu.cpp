
#include "main_menu.h"
#include "environment.h"

namespace Ign
{

void MainMenu::RegisterComponent( Context * context )
{
    context->RegisterFactory<MainMenu>();
}

MainMenu::MainMenu( Context * context )
  : LogicComponent( context )
{
}

MainMenu::~MainMenu()
{
}

void MainMenu::Start()
{
    SubscribeToEvents();
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
    Scene * s = GetScene();
    if ( !s )
        return;

    auto * e = s->GetComponent<Environment>();
    if ( !e )
        return;

    e->StartServer();

    SetVisible( false );
}

void MainMenu::HandleConnect( StringHash event, VariantMap & args )
{
    Scene * s = GetScene();
    if ( !s )
        return;

    auto * e = s->GetComponent<Environment>();
    if ( !e )
        return;

    ClientDesc c;
    c.login_ = "login";
    c.password_ = "password";
    e->Connect( c );

    SetVisible( false );
}

void MainMenu::HandleSettings( StringHash event, VariantMap & args )
{
    SetVisible( false );
}

void MainMenu::HandleExit( StringHash event, VariantMap & args )
{
    Engine * e = GetSubsystem<Engine>();
    e->Exit();
}


}





