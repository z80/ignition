
extends Node

const BODIES_GROUP_NAME: String    = "bodies"
const SUPER_BODIES_GROUP_NAME: String    = "super_bodies"
const REF_FRAME_PHYSICS_GROUP_NAME: String = "ref_frame_physics"
const PROVIDERS_GROUP_NAME: String = "providers"
const CELESTIAL_BODIES_NAME: String = "celestial_bodies"
const PLANETS_GROUP_NAME: String   = "spheres"
const SUN_GROUP_NAME: String       = "sun"

const DEBUG: bool = true
const RF_JUMP_DISTANCE: float  = 50.0
const RF_SPLIT_DISTANCE: float = 100.0
const RF_MERGE_DISTANCE: float = 80.0
# Switching ref frames.
# Switches to orbiting with atmosphere height+rf_change_DELTA. 
# And back to surface rf with atmosphere height-rf_change_DELTA.
const RF_CHANGE_DELTA = 5.0



const BODY_EXCLUDE_DIST: float = 100.0
const BODY_INCLUDE_DIST: float = 80.0

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



# Node sizes when stacking parts together.
const NODE_SIZE_SMALL: float  = 0.25
const NODE_SIZE_MEDIUM: float = 0.5
const NODE_SIZE_LARGE: float  = 1.0




