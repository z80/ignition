
extends TileBase

func contruct():
	var meshes = .recomute()
	var land_mesh  = meshes[0]
	var water_mesh = meshes[1]
	
	self.mesh  = land_mesh
	$WaterMesh.mesh = water_mesh
	






