# Class: VoxelLodTerrain

Inherits: Spatial

_Godot version: 3.2_


## Online Tutorials: 



## Constants:


## Properties:

#### � int collision_lod_count

`set_collision_lod_count (value)` setter

`get_collision_lod_count ()` getter


#### � bool generate_collisions

`set_generate_collisions (value)` setter

`get_generate_collisions ()` getter


#### � int lod_count

`set_lod_count (value)` setter

`get_lod_count ()` getter


#### � float lod_split_scale

`set_lod_split_scale (value)` setter

`get_lod_split_scale ()` getter


#### � Material material

`set_material (value)` setter

`get_material ()` getter


#### � VoxelStream stream

`set_stream (value)` setter

`get_stream ()` getter


#### � int view_distance

`set_view_distance (value)` setter

`get_view_distance ()` getter


#### � NodePath viewer_path

`set_viewer_path (value)` setter

`get_viewer_path ()` getter



## Methods:

#### � Dictionary debug_get_block_info ( Vector3 block_pos, int lod )  const


#### � Array debug_get_octrees (  )  const


#### � Array debug_print_sdf_top_down ( Vector3 center, Vector3 extents )  const


#### � Array debug_raycast_block ( Vector3 origin, Vector3 dir )  const


#### � int get_block_region_extent (  )  const


#### � int get_block_size (  )  const


#### � Dictionary get_statistics (  )  const


#### � VoxelTool get_voxel_tool (  ) 


#### � void save_modified_blocks (  ) 


#### � Vector3 voxel_to_block_position ( Vector3 lod_index, int arg1 )  const



## Signals:


---
* [Class List](Class_List.md)
* [Doc Index](../01_get-started.md)

_Generated on Aug 10, 2020_
