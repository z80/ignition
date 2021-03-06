
#ifndef __GLOBAL_3DPARTY_H_
#define __GLOBAL_3DPARTY_H_

#include <Urho3D/Container/Str.h>
using namespace Urho3D;

namespace Ign
{

// Control bits we define
static const unsigned CTRL_FORWARD  = 1 << 0;   //1
static const unsigned CTRL_BACK     = 1 << 1;   //2
static const unsigned CTRL_LEFT     = 1 << 2;   //4
static const unsigned CTRL_RIGHT    = 1 << 3;   //8
static const unsigned CTRL_JUMP     = 1 << 4;   //16
static const unsigned CTRL_ACTION   = 1 << 5;   //32
static const unsigned CTRL_ALT_ACTION = 1 << 6; //32
static const unsigned CTRL_SPRINT   = 1 << 7;   //64
static const unsigned CTRL_UP       = 1 << 8;   //128
static const unsigned CTRL_ZOOM_IN  = 1 << 9;  //
static const unsigned CTRL_ZOOM_OUT = 1 << 10;
static const unsigned CTRL_CENTER   = 1 << 11;
static const unsigned CTRL_TRIGGER  = 1 << 12;
static const unsigned CTRL_CW       = 1 << 13;
static const unsigned CTRL_CCW      = 1 << 14;

static const unsigned COLLISION_MASK_PLAYER = 1 << 0; //1
static const unsigned COLLISION_MASK_CHECKPOINT = 1 << 1; //2 
static const unsigned COLLISION_MASK_OBSTACLES = 1 << 2; //4

static const String APPLICATION_FONT = "Fonts/Muli-Regular.ttf";

static const float GAMMA_MAX_VALUE = 2.0f;

static const String DOCUMENTS_DIR = "Ignition";

static const StringHash IGN_ZOOM_VALUE( "IgnZoomValue" );

}

#endif

