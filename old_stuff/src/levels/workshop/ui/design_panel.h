
#ifndef __DESIGN_PANEL_H_
#define __DESIGN_PANEL_H_

#include <Urho3D/Urho3DAll.h>
#include "BaseWindow.h"

namespace Osp
{

class DesignPanel: public BaseWindow
{
    URHO3D_OBJECT( DesignPanel, Object )
public:
    DesignPanel(Context* context);

    ~DesignPanel();
    void Init();
protected:
    void Create();

private:
    void SubscribeToEvents();

    SharedPtr<Button> _newGameButton;
    SharedPtr<Button> _exitWindow;
    SharedPtr<Window> _baseWindow;

    Button* CreateButton(const String& text, int width, IntVector2 position);
};

}

#endif
