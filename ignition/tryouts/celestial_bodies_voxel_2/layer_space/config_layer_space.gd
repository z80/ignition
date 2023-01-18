
extends Resource


export(float) var scale_divider = 10000.0
export(float) var event_horizon = 2000.0
export(float) var max_level     = 6

# How deep focal depth is when height over surface is 0.
export(float) var min_relative_focal_depth  = 0.05
# How far should focal point move in order to trigger rebuild.
export(float) var relative_rebuild_dist     = 0.3


