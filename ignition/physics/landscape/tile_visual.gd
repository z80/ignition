
extends TileBase

func construct():
	super.construct()
	var meshes = recompute()
	var land_mesh  = meshes[0]
	var water_mesh = meshes[1]
	
	self.mesh  = land_mesh
	$WaterMesh.mesh = water_mesh
	






