
#ifndef __LAUNCH_SITE_H_
#define __LAUNCH_SITE_H_

#include <Urho3D/Urho3DAll.h>
#include "item_base.h"

namespace Osp
{

class Player;

using namespace Urho3D;

class LaunchSite: public ItemBase
{
    URHO3D_OBJECT( LaunchSite, ItemBase )
public:
    LaunchSite( Context * ctx );
    ~LaunchSite();

    void Start() override;

private:
    void createPlayer();
};

}


#endif





