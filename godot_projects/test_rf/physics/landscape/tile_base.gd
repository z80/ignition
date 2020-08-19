extends MeshInstance
class_name TileBase

var index_x: int = 0
var index_z: int = 0
var rebuild: bool = true
var available: bool = true


# Need to redefine this one. Create either visual instance or 
# Collision shape here.
func construct():
	pass

func recompute():
	var ls = get_parent()
	if not ls:
		return
	#if not (ls is MeshInstance):
	#	return
	var origin_x: float = ls.size * float(index_x)
	var origin_z: float = ls.size * float(index_z)
	var verts_ext = []
	var res_plus_one = (ls.resolution+1)
	var res_plus_one2 = res_plus_one*res_plus_one
	verts_ext.resize( res_plus_one2 )
	var res_minus_one: int = ls.resolution - 1
	
	var ind: int = 0
	for iz in range( res_plus_one ):
		var z = float(iz) * ls.size / float(res_minus_one)
		var probe_z: float = z + origin_z
		for ix in range( res_plus_one ):
			var x = float(ix) * ls.size / float(res_minus_one) + origin_x
			var probe_x: float = x + origin_x
			var h: float = ls.height( probe_x, probe_z )
			verts_ext[ind] = Vector3( x, h, z )
			ind += 1
	
	var verts_qty: int = res_minus_one*res_minus_one * 6
	var vertices = PoolVector3Array()
	var normals  = PoolVector3Array()
	var tangents = PoolRealArray()
	var colors   = PoolColorArray()
	var uvs      = PoolVector2Array()
	vertices.resize( verts_qty )
	normals.resize( verts_qty )
	tangents.resize( verts_qty*4 )
	colors.resize( verts_qty )
	uvs.resize( verts_qty )
	
	var col: Color = Color( 1.0, 1.0, 1.0, 1.0 )
	ind = 0
	for iz in range( res_minus_one ):
		var line_qty: int = iz * res_plus_one
		var uv_v_0 = float(iz) / float(res_minus_one)
		var uv_v_1 = float(iz+1) / float(res_minus_one)
		for ix in range( res_minus_one ):
			var uv_u_0 = float(ix) / float(res_minus_one)
			var uv_u_1 = float(ix+1) / float(res_minus_one)
			var ind00 = line_qty + ix
			var ind01 = ind00 + 1
			var ind02 = ind01 + 1
			var ind10 = ind00 + res_plus_one
			var ind11 = ind10 + 1
			var ind12 = ind11 + 1
			var ind20 = ind10 + res_plus_one
			var ind21 = ind20 + 1
			var ind22 = ind21 + 1
			
			var v00: Vector3 = verts_ext[ind00]
			var v01: Vector3 = verts_ext[ind01]
			var v02: Vector3 = verts_ext[ind02]
			var v10: Vector3 = verts_ext[ind10]
			var v11: Vector3 = verts_ext[ind11]
			var v12: Vector3 = verts_ext[ind12]
			var v20: Vector3 = verts_ext[ind20]
			var v21: Vector3 = verts_ext[ind21]
			var v22: Vector3 = verts_ext[ind22]
			
			var vb = v01 - v00
			var va = v10 - v00
			var n00 = va.cross(vb)
			n00 = n00.normalized()
			var t00: Vector3 = va.normalized()
			
			vb = v02 - v01
			va = v11 - v01
			var n01 = va.cross(vb)
			n01 = n01.normalized()
			var t01: Vector3 = va.normalized()
			
			vb = v12 - v11
			va = v21 - v11
			var n11 = va.cross(vb)
			n11 = n01.normalized()
			var t11: Vector3 = va.normalized()

			vb = v11 - v10
			va = v20 - v10
			var n10 = va.cross(vb)
			n10 = n01.normalized()
			var t10: Vector3 = va.normalized()
			
			
			var uv00 := Vector2(uv_u_0, uv_v_0)
			var uv01 := Vector2(uv_u_1, uv_v_0)
			var uv11 := Vector2(uv_u_1, uv_v_1)
			var uv10 := Vector2(uv_u_0, uv_v_1)
			
			vertices[ind] = v00
			normals[ind]  = n00
			tangents[ind*4]   = t00.x
			tangents[ind*4+1] = t00.y
			tangents[ind*4+2] = t00.z
			tangents[ind*4+3] = 1.0
			colors[ind]   = col
			uvs[ind]      = uv00
			ind += 1
			vertices[ind] = v01
			normals[ind]  = n01
			tangents[ind*4]   = t01.x
			tangents[ind*4+1] = t01.y
			tangents[ind*4+2] = t01.z
			tangents[ind*4+3] = 1.0
			colors[ind]   = col
			uvs[ind]      = uv01
			ind += 1
			vertices[ind] = v10
			normals[ind]  = n10
			tangents[ind*4]   = t10.x
			tangents[ind*4+1] = t10.y
			tangents[ind*4+2] = t10.z
			tangents[ind*4+3] = 1.0
			colors[ind]   = col
			uvs[ind]      = uv10
			ind += 1
			
			vertices[ind] = v01
			normals[ind]  = n01
			tangents[ind*4]   = t01.x
			tangents[ind*4+1] = t01.y
			tangents[ind*4+2] = t01.z
			tangents[ind*4+3] = 1.0
			colors[ind]   = col
			uvs[ind]      = uv01
			ind += 1
			vertices[ind] = v11
			normals[ind]  = n11
			tangents[ind*4]   = t11.x
			tangents[ind*4+1] = t11.y
			tangents[ind*4+2] = t11.z
			tangents[ind*4+3] = 1.0
			colors[ind]   = col
			uvs[ind]      = uv11
			ind += 1
			vertices[ind] = v10
			normals[ind]  = n10
			tangents[ind*4]   = t10.x
			tangents[ind*4+1] = t10.y
			tangents[ind*4+2] = t10.z
			tangents[ind*4+3] = 1.0
			colors[ind]   = col
			uvs[ind]      = uv10
			ind += 1
	
	# Initialize the ArrayMesh.
	var arr_mesh = ArrayMesh.new()
	var arrays = []
	arrays.resize(ArrayMesh.ARRAY_MAX)
	arrays[ArrayMesh.ARRAY_VERTEX] = vertices
	arrays[ArrayMesh.ARRAY_NORMAL] = normals
	arrays[ArrayMesh.ARRAY_TANGENT] = tangents
	arrays[ArrayMesh.ARRAY_COLOR]  = colors
	arrays[ArrayMesh.ARRAY_TEX_UV] = uvs
	arrays[ArrayMesh.ARRAY_TEX_UV2] = uvs
	# Create the Mesh.
	arr_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
	
	
	# Create water.
	var water_norm: Vector3 = Vector3.UP;
	for i in range( verts_qty ):
		var v: Vector3 = vertices[i]
		v.y = 0.0
		vertices[i] = v
		normals[i] = water_norm
	var water_arr_mesh = ArrayMesh.new()
	arrays = []
	arrays.resize(ArrayMesh.ARRAY_MAX)
	arrays[ArrayMesh.ARRAY_VERTEX] = vertices
	arrays[ArrayMesh.ARRAY_NORMAL] = normals
	arrays[ArrayMesh.ARRAY_TANGENT] = tangents
	arrays[ArrayMesh.ARRAY_COLOR]  = colors
	arrays[ArrayMesh.ARRAY_TEX_UV] = uvs
	arrays[ArrayMesh.ARRAY_TEX_UV2] = uvs
	# Create the Mesh.
	water_arr_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
	
	rebuild = false
	
	return [arr_mesh, water_arr_mesh]
	
	






