
extends Resource


@export var scale_divider: float = 10000.0
@export var event_horizon: float = 2000.0
@export var max_level: float     = 6

# Some limits on how many subdivisions to perform.
@export var precision: float   = 0.01
@export var max_nodes_qty: int = 200000

# How deep focal depth is when height over surface is 0.
@export var min_relative_focal_depth: float  = 0.05
# How far should focal point move in order to trigger rebuild.
@export var relative_rebuild_dist: float     = 0.3


@export var surface_node_size: float = 100.0


