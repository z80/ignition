
extends Node

const BODIES_GROUP_NAME: String    = "bodies"
const PROVIDERS_GROUP_NAME: String = "providers"
const SPHERES_GROUP_NAME: String   = "spheres"
const SUN_GROUP_NAME: String       = "sun"

const DEBUG := true
const RF_JUMP_DISTANCE: float  = 5.0
const RF_SPLIT_DISTANCE: float = 10.0
const RF_MERGE_DISTANCE: float = 8.0

const BODY_EXCLUDE_DIST: float = 10.0
const BODY_INCLUDE_DIST: float = 8.0

# Icon shows up only if closer than this distance
const INTERACT_ICON_DIST: float = 30.0
# It shows up if mouse is closer than this distance on screen.
# It is in screen heights.
const INTERACT_ICON_SCREEN_DIST: float = 0.2

# Positional grid size.
const CONSTRUCTION_GRID_SZ: float = 0.1
# Rotational discretization.
const CONSTRUCTION_ROT_SNAP: float = 5.0 * 3.1415926535 / 180.0

# Activation distance
const CONSTRUCTION_ACTIVATE_DIST: float = 15.0
# Deactivation distance
const CONSTRUCTION_DEACTIVATE_DIST: float = 20.0

# Where create new blocks relative to player.
const CONSTRUCTION_CREATE_AT: Vector3 = Vector3( 0.0, 2.0, -5.0 )
