
extends Node

const BODIES_GROUP_NAME: String    = "bodies"
const PROVIDERS_GROUP_NAME: String = "providers"

const DEBUG := true
const RF_JUMP_DISTANCE: float  = 10.0
const RF_SPLIT_DISTANCE: float = 50.0
const RF_MERGE_DISTANCE: float = 60.0

const INTERACT_ICON_DIST: float = 30.0

# Positional grid size.
const CONSTRUCTION_GRID_SZ: float = 0.1
# Rotational discretization.
const CONSTRUCTION_ROT_SNAP: float = 5.0 * 3.1415926535 / 180.0
