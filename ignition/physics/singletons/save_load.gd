

static func saved_files():
	var dir = Directory.new()
	ensure_save_folder_exists( dir )
	dir.change_dir( Constants.SAVE_DIR_NAME )
	
	var ret: Array = []
	dir.list_dir_begin( true, true )
	var file_name: String = dir.get_next()
	while file_name != "":
		if not dir.current_is_dir():
			ret.push_back( file_name )
		file_name = dir.get_next()
	
	return ret


static func ensure_save_folder_exists( dir: Directory ):
	var exists: bool = dir.dir_exists( Constants.SAVE_DIR_NAME )
	if not exists:
		var err: int = dir.make_dir( Constants.SAVE_DIR_NAME )
		print( "save folder creation error code: ", err )



static func full_file_path( fname: String ):
	var ret: String = Constants.SAVE_DIR_NAME + "/" + fname
	return ret



static func serialize_all( n: Node ):
	var data: Dictionary = {}
	
	var root_data: Dictionary = serialize_root_node()
	data["root"] = root_data
	
	var stars_data: Dictionary      = serialize_stars( n )
	data["stars"] = stars_data
	
	var planets_data: Dictionary    = serialize_planets( n )
	data["planets"] = planets_data
	
	var ref_frames_data: Dictionary = serialize_ref_frames_physics( n )
	data["ref_frames"] = ref_frames_data
	
	var bodies_data: Dictionary     = serialize_bodies( n )
	data["bodies"] = bodies_data
	
	var camera_data: Dictionary     = serialize_camera()
	data["camera"] = camera_data
	
	return data




static func deserialize_all( n: Node, data: Dictionary ):
	destroy_all_bodies( n )
	destroy_all_assemblies( n )
	destroy_all_ref_frames_physics( n )
	
	var stars_data: Dictionary = data.stars
	var ret: bool = deserialize_stars( n, stars_data )
	if not ret:
		return false
	
	var planets_data: Dictionary = data.planets
	ret = deserialize_planets( n, planets_data )
	if not ret:
		return false
	
	var ref_frames_data: Dictionary = data.ref_frames
	ret = deserialize_ref_frames_physics( n, ref_frames_data )
	if not ret:
		return false
	
	var bodies_data: Dictionary = data.bodies
	ret = deserialize_bodies( n, bodies_data )
	if not ret:
		return false
	
	var physics_data: Dictionary = data.physics
	ret = deserialize_root_node( physics_data )
	if not ret:
		return false
	
	var camera_data: Dictionary = data.camera
	ret = deserialize_camera( camera_data )
	if not ret:
		return false
	
	return true




static func serialize_root_node():
	var data: Dictionary = RootScene.ref_frame_root.serialize()
	return data


static func deserialize_root_node( data: Dictionary ):
	var ret: bool = RootScene.ref_frame_root.deserialize( data )
	return ret


# Planets and the sun are fixed. No need to establish or 
# keep parent/child relationship.
# Argument can be any valid node in the scene tree. It is 
# used to only get access to the scene root.
static func serialize_stars( n: Node ):
	var suns: Array = n.get_tree().get_nodes_in_group( Constants.SUN_GROUP_NAME )
	var suns_data: Dictionary = {}
	for sun in suns:
		var data: Dictionary = sun.serialize()
		var path: String = sun.get_path()
		var all_data: Dictionary = {
			data = data, 
			path = path
		}
		var name: String = sun.name
		suns_data[name] = all_data
	
	return suns_data



static func deserialize_stars( n: Node, stars_data: Dictionary ):
	for name in stars_data:
		var all_data: Dictionary = stars_data[name]
		var path: String = all_data["path"]
		# This reports error if the node is not found.
		# So it is net additionally reported here in the code.
		var star = n.get_node( path )
		if star == null:
			return false
		var data: Dictionary = all_data["data"]
		var ret: bool = star.deserialize( data )
		if not ret:
			return false
	
	return true




static func serialize_planets( n: Node ):
	var planets = n.get_tree().get_nodes_in_group( Constants.PLANETS_GROUP_NAME )
	var planets_data: Dictionary = {}
	for p in planets:
		var data: Dictionary = p.serialize()
		var path: String = p.get_path()
		var name: String = p.name
		var all_data: Dictionary = {
			data=data, 
			path=path
		}
		planets_data[name] = all_data
	
	return planets_data



static func deserialize_planets( n: Node, planets_data: Dictionary ):
	for name in planets_data:
		var all_data: Dictionary = planets_data[name]
		var path: String = all_data["path"]
		# This reports error if the node is not found.
		# So it is net additionally reported here in the code.
		var planet = n.get_node( path )
		if planet == null:
			continue
		var data: Dictionary = all_data["data"]
		var ret: bool = planet.deserialize( data )
		if not ret:
			return false
	
	return true



static func serialize_ref_frames_physics( n: Node ):
	var ref_frames: Array = n.get_tree().get_nodes_in_group( Constants.REF_FRAME_PHYSICS_GROUP_NAME )
	var rfs_data: Dictionary = {}
	for rf in ref_frames:
		var data: Dictionary = rf.serialize()
		var name: String = rf.name
		var parentpath: String = rf.get_parent().get_path()
		var all_data: Dictionary = {
			parentpath = parentpath, 
			data = data
		}
		rfs_data[name] = all_data
	
	return rfs_data



static func destroy_all_ref_frames_physics( n: Node ):
	var ref_frames: Array = n.get_tree().get_nodes_in_group( Constants.REF_FRAME_PHYSICS_GROUP_NAME )
	for rf in ref_frames:
		var _p: Node = rf.get_parent()
		rf.queue_free()
		rf.name = rf.name + "_t_be_deleted"



# They can't belong to each other, only to planets or stars.
# So can be sure that path exists.
static func deserialize_ref_frames_physics( n: Node, rf_data: Dictionary ):
	for name in rf_data:
		var all_data: Dictionary = rf_data[name]
		var parentpath: String = all_data.parentpath
		var rf: Node = RootScene.ref_frame_root.create_ref_frame_physics()
		rf.name = name
		var parent: Node = n.get_node( parentpath )
		if parent != null:
			parent.add_child( rf )

	for name in rf_data:
		var all_data: Dictionary = rf_data[name]
		var parentpath: String = all_data.parentpath
		var path: String = parentpath + "/" + name
		var rf: Node = n.get_node( path )
		var data: Dictionary = all_data.data
		var ret: bool = rf.deserialize( data )
		if not ret:
			return false
	
	return true



static func serialize_bodies( n: Node ):
	var bodies: Array = n.get_tree().get_nodes_in_group( Constants.BODIES_GROUP_NAME )
	var bodies_data: Dictionary = {}
	for b in bodies:
		# Don't save SurfaceProvider and PartAssembly
		var cl: String = b.get_class()
		if (cl == "SurfaceProvider") or (cl == "PartAssembly"):
			continue
		var data: Dictionary    = b.serialize()
		var filename: String    = b.filename
		var name: String        = b.name
		var parent_path: String = b.get_parent().get_path()
		var all_data: Dictionary = {
			filename = filename, 
			parentpath = parent_path, 
			data = data
		}
		bodies_data[name] = all_data
	
	return bodies_data



static func destroy_all_bodies( n: Node ):
	var bodies: Array = n.get_tree().get_nodes_in_group( Constants.BODIES_GROUP_NAME )
	for b in bodies:
		b.queue_free()
		b.name = b.name + "_to_be_deleted"
		if b._visual != null:
			b._visual.name = b._visual.name + "_to_be_deleted"
		if b._physical != null:
			b._physical.name = b._physical.name + "_to_be_deleted"


static func destroy_all_assemblies( n: Node ):
	var assemblies: Array = n.get_tree().get_nodes_in_group( Constants.BODY_ASSEMBLIES_GROUP_NAME )
	for b in assemblies:
		b.queue_free()
		b.name = b.name + "_to_be_deleted"



static func deserialize_bodies( n: Node, bodies_data: Dictionary ):
	var bodies: Array = []
	for name in bodies_data:
		var all_data: Dictionary = bodies_data[name]
		var filename: String = all_data.filename
		var parentpath = all_data.parentpath
		var B = load( filename )
		var b = B.instance()
		b.name = name
		var p: Node = n.get_node( parentpath )
		p.add_child( b )
		b.init()
		# Need to overwrite visual name in order to make 
		# connecting nodes to be detected correctly.
		# This is argualble that this is the best place to do it.
		# But I can't come up with more elegant solution right now.
		var data: Dictionary = all_data.data
		if data.has("visual_name"):
			var visual_name: String = data["visual_name"]
			b._visual.name = visual_name
		
		# Parts hame attachments.
		# Need to create attachments before they are initialized.
		var part: Part = b as Part
		if part != null:
			Part.create_attachments( b, data )
		
		bodies.push_back( b )
	
	for name in bodies_data:
		var all_data: Dictionary = bodies_data[name]
		var parentpath: String = all_data.parentpath
		var path: String = parentpath + "/" + name
		var b: Node = n.get_node( path )
		var data: Dictionary = all_data.data
		var ret: bool = b.deserialize( data )
		if not ret:
			return false
	
	# By now all properties are applied. Now can activate/deactivate bodies based 
	# on their mode.
	for b in bodies:
		b.activate()
	
	return true
	





# And the remaining object currently existing is the camera.
# It is not destroyable. So no need to save name.
static func serialize_camera():
	var c: RefFrameNode = RootScene.ref_frame_root.player_camera
	var data: Dictionary = c.serialize()
	var _name: String = c.name
	var _filename: String = c.filename
	var parentpath = c.get_parent().get_path()
	var camera_data: Dictionary = {
		data = data,
		parentpath = parentpath
	}
	
	return camera_data



static func deserialize_camera( camera_data: Dictionary ):
	var c: RefFrameNode = RootScene.ref_frame_root.player_camera
	var _parentpath: String = camera_data.parentpath
	var data: Dictionary = camera_data.data
	var ret: bool = c.deserialize( data )
	if not ret:
		return false
	
	return true




#
#static func serialize( n: Node ):
#	var cl: String = n.get_class()
#	var data: Dictionary
#	if cl != "SelectialSurface":
#		data = serialize_celestial_surface( n )
#	else:
#		data = serialize_common( n )
#	return data
#
#
#
#
#static func serialize_common( n: Node ):
#	var filename: String = n.filename
#	print( "serializing \'", n.name, "\'" )
#	var data_self: Dictionary
#	var has_serialize: bool = n.has_method( "serialize" )
#	print( "has serialization: ", has_serialize )
#	if has_serialize:
#		data_self = n.serialize()
#	else:
#		data_self = {}
#	print( "serializing children for \'", n.name, "\'" )
#	var data_children: Dictionary  = serialize_children( n, [] )
#	var data: Dictionary = {
#		filename = filename, 
#		data     = data_self, 
#		children = data_children
#	}
#	print( "serialization for \'", n.name, "\' is done" )
#	return data
#
#
#
#static func serialize_celestial_surface( n: Node ):
#	var filename: String = n.filename
#	print( "serializing \'", n.name, "\'" )
#	var data_self: Dictionary
#	var has_serialize: bool = n.has_method( "serialize" )
#	print( "has serialization: ", has_serialize )
#	data_self = n.serialize()
#	print( "serializing children for \'", n.name, "\'" )
#	var translation_rf: Node = n.translation_rf()
#	var rotation_rf: Node    = n.rotation_rf()
#	var data_children_translation: Dictionary = serialize_children( translation_rf, [rotation_rf] )
#	var data_children_rotation: Dictionary    = serialize_children( rotation_rf, [] )
#	var data: Dictionary = {
#		filename = filename, 
#		data     = data_self, 
#		children_translation = data_children_translation, 
#		children = data_children_rotation
#	}
#	print( "serialization for \'", n.name, "\' is done" )
#	return data
#
#
#
#
#static func serialize_children( n: Node, exclusion: Array ):
#	print( "started serializing children for \'", n.name, "\'" )
#	var children_data: Dictionary = {}
#	var qty: int = n.get_child_count()
#	print( "number of children to serialize: ", qty )
#	for i in range( qty ):
#		print( "serializing child #: ", i )
#		var ch: Node = n.get_child( i )
#		if ch in exclusion:
#			continue
#		print( "child name: \'", ch.name, "\'" )
#		var data: Dictionary = serialize( ch )
#		var name: String = ch.name
#		children_data[name] = data
#
#	return children_data
#
#
#
#
#static func deserialize( node: Node, data: Dictionary ):
#	var cl: String = node.get_class()
#	var ret: bool
#	if cl == "CelestialSurface":
#		ret = deserialize_celestial_surface( node, data )
#	else:
#		ret = deserialize_common( node, data )
#
#	return ret
#
#
#
#static func deserialize_common( node: Node, data: Dictionary ):
#	var has_data: bool = data.has( "data" )
#	var has_deserialize: bool = node.has_method( "deserialize" )
#	if has_data and has_deserialize:
#		var node_data: Dictionary = data["data"]
#		print( "deserializing \'", node.name, "\' ..." )
#		var ret = node.deserialize( node_data )
#		print( "deserialization result for \'", node.name, "\': ", ret )
#	var has_children: bool = data.has( "children" )
#	if has_children:
#		var data_children: Dictionary = data["children"]
#		print( "deserializing children of \'", node.name, "\' ..." )
#		var ret = deserialize_children( node, data_children )
#		print( "deserialization result for children of \'", node.name, "\': ", ret )
#		return ret
#	else:
#		print( "no children to serialize for node \'", node.name, "\'" )
#	return true
#
#
#static func deserialize_celestial_surface( node: Node, data: Dictionary ):
#	var has_data: bool = data.has( "data" )
#	var has_deserialize: bool = node.has_method( "deserialize" )
#	if has_data and has_deserialize:
#		var node_data: Dictionary = data["data"]
#		print( "deserializing \'", node.name, "\' ..." )
#		var ret = node.deserialize( node_data )
#		print( "deserialization result for \'", node.name, "\': ", ret )
#	var has_children: bool = data.has( "children" )
#	if has_children:
#		var data_children: Dictionary = data["children"]
#		print( "deserializing children of \'", node.name, "\' ..." )
#		var ret = deserialize_children( node, data_children )
#		print( "deserialization result for children of \'", node.name, "\': ", ret )
#		return ret
#	else:
#		print( "no children to serialize for node \'", node.name, "\'" )
#	return true
#
#
#
#
#static func deserialize_children( parent: Node, data: Dictionary ):
#	print( "creating children for \'", parent.name, "\'" )
#	var has_children: bool = data.has( "children" )
#	print( "has_children: ", has_children )
#	var children_data: Dictionary = data["children"]
#	for name in data:
#		print( "trying to create \'", name, "\'" )
#		var child_data_full: Dictionary = data[name]
#		# We must have "node" entry and optinally "children" entry.
#		var has_filename: bool = child_data_full.has( "filename" )
#		print( "has filename: ", has_filename )
#		if has_filename :
#			var filename: String = child_data_full["filename"]
#			var R: Resource = load( filename )
#			var n: Node = R.instance()
#			n.name = name
#			print( "created the instance successfully" )
#			parent.add_child( n )
#			print( "deserializing \'", name, "\'" )
#			var ret = deserialize( n, child_data_full )
#			print( "deserialization result for \'", name, "\': ", ret )
#


