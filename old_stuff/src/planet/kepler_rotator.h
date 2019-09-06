
#ifndef __KEPLER_ROTATOR_H_
#define __KEPLER_ROTATOR_H_

#include <Urho3D/Urho3DAll.h>
#include "game_data.h"
#include "data_types.h"

namespace Osp
{

class KeplerRotator: public ItemBase
{
    URHO3D_OBJECT( KeplerRotator, ItemBase )
public:
    KeplerRotator( Context * ctx );
    ~KeplerRotator();

    void launch( Timestamp periodSec, Float yaw=0.0, Float pitch=0.0, Float roll=0.0 );

    void Start() override;
    void Update( float dt ) override;

public:
    void computeBaseRotation();
    SharedPtr<GameData>   gameData;
    Float yaw, pitch, roll;
    Timestamp period;
    Quaterniond qBase;
    bool active;
};

}

#endif


