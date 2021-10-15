#include <Urho3D/Urho3DAll.h>

// Control bits we define
static const unsigned CTRL_FORWARD = 1 << 0; //1
static const unsigned CTRL_BACK    = 1 << 2; //2
static const unsigned CTRL_LEFT    = 1 << 3; //4
static const unsigned CTRL_RIGHT   = 1 << 4; //8
static const unsigned CTRL_JUMP    = 1 << 5; //16
static const unsigned CTRL_ACTION  = 1 << 6; //32
static const unsigned CTRL_SPRINT  = 1 << 7; //64
static const unsigned CTRL_UP      = 1 << 8; //128
static const unsigned CTRL_CCW     = 1 << 9; //256
static const unsigned CTRL_CW      = 1 << 10; //512
static const unsigned CTRL_THRUST_UP = 1 << 11; //1024
static const unsigned CTRL_THRUST_DN = 1 << 12; // 2048

static const unsigned COLLISION_MASK_PLAYER = 1 << 0; //1
static const unsigned COLLISION_MASK_CHECKPOINT = 1 << 1; //2 
static const unsigned COLLISION_MASK_OBSTACLES = 1 << 2; //4

static String APPLICATION_FONT = "Fonts/Muli-Regular.ttf";

static const float GAMMA_MAX_VALUE = 2.0f;

static const String DOCUMENTS_DIR = "ProjectTemplate";
