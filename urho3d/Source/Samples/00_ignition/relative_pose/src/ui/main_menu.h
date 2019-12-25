
#ifndef __MAIN_MENU_H_
#define __MAIN_MENU_H_

#include "Urho3D/Urho3DAll.h"
#include "BaseWindow.h"

namespace Ign
{

class MainMenu: public BaseWindow
{
    URHO3D_OBJECT( MainMenu, BaseWindow )
public:
    static void RegisterObject( Context * context );

    MainMenu( Context * context );
    ~MainMenu();

    void Init() override;
    void SetVisible( bool en );

private:
    void Create() override;
protected:
    void Dispose() override;

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




