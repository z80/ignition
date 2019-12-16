
#ifndef __MAIN_MENU_H_
#define __MAIN_MENU_H_

#include "Urho3D/Urho3DAll.h"

namespace Ign
{

class MainMenu: public LogicComponent
{
    URHO3D_OBJECT( MainMenu, LogicComponent )
public:
    static void RegisterComponent( Context * context );

    MainMenu( Context * context );
    ~MainMenu();

    void Start() override;
    void SetVisible( bool en );

protected:
    void SubscribeToEvents();

    // Event processing routines.
    void HandleNewGame( StringHash event, VariantMap & args );
    void HandleConnect( StringHash event, VariantMap & args );
    void HandleSettings( StringHash event, VariantMap & args );
    void HandleExit( StringHash event, VariantMap & args );


    SharedPtr<UIElement> mainWnd_;
};


}




#endif




