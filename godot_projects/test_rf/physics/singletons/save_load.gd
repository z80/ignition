



static func serialize_all( n: Node ):
	var data: Dictionary = {}
	var camera_data: Dictionary     = serialize_camera()
	var bodies_data: Dictionary     = serialize_bodies( n )
	var ref_frames_data: Dictionary = serialize_ref_frames_physics( n )
	var planets_data: Dictionary    = serialize_planets( n )
	var suns_data: Dictionary       = serialize_suns( n )


# Planets and the sun are fixed. No need to establish or 
# keep parent/child relationship.
# Argument can be any valid node in the scene tree. It is 
# used to only get access to the scene root.
static func serialize_suns( n: Node ):
	var suns: Array = n.get_tree().get_nodes_in_group( Constants.SUN_GROUP_NAME )
	var suns_data: Dictionary = {}
	for sun in suns:
		var data: Dictionary = sun.serialize()
		var name: String = sun.name
		suns_data[name] = data
	
	return suns_data



static func serialize_planets( n: Node ):
	var planets = n.get_tree().get_nodes_in_group( Constants.PLANETS_GROUP_NAME )
	var planets_data: Dictionary = {}
	for p in planets:
		var data: Dictionary = p.serialize()
		var name: String = p.name
		planets_data[name] = data
	
	return planets_data


static func serialize_bodies( n: Node ):
	var bodies: Array = n.get_tree().get_nodes_in_group( Constants.BODIES_GROUP_NAME )
	var bodies_data: Dictionary = {}
	for b in bodies:
		var data: Dictionary    = b.serialize()
		var filename: String    = n.filename
		var name: String        = b.name
		var parent_path: String = b.get_parent().get_path()
		var all_data: Dictionary = {
			filename = filename, 
			parentpath = parent_path, 
			data = data
		}
		bodies_data[name] = all_data
	
	return bodies_data




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



# And the remaining object currently existing is the camera.
# It is not destroyable. So no need to save name.
static func serialize_camera():
	var c: Camera = PhysicsManager.camera
	var data: Dictionary = c.serialize()
	var name: String = c.name
	var filename: String = c.filename
	var parentpath = c.get_parent().get_path()
	var camera_data: Dictionary = {
		data = data,
		parentpath = parentpath
	}
	
	return camera_data





static func serialize( n: Node ):
	var cl: String = n.get_class()
	var data: Dictionary
	if cl != "SelectialSurface":
		data = serialize_celestial_surface( n )
	else:
		data = serialize_common( n )
	return data




static func serialize_common( n: Node ):
	var filename: String = n.filename
	print( "serializing \'", n.name, "\'" )
	var data_self: Dictionary
	var has_serialize: bool = n.has_method( "serialize" )
	print( "has serialization: ", has_serialize )
	if has_serialize:
		data_self = n.serialize()
	else:
		data_self = {}
	print( "serializing children for \'", n.name, "\'" )
	var data_children: Dictionary  = serialize_children( n, [] )
	var data: Dictionary = {
		filename = filename, 
		data     = data_self, 
		children = data_children
	}
	print( "serialization for \'", n.name, "\' is done" )
	return data



static func serialize_celestial_surface( n: Node ):
	var filename: String = n.filename
	print( "serializing \'", n.name, "\'" )
	var data_self: Dictionary
	var has_serialize: bool = n.has_method( "serialize" )
	print( "has serialization: ", has_serialize )
	data_self = n.serialize()
	print( "serializing children for \'", n.name, "\'" )
	var translation_rf: Node = n.translation_rf()
	var rotation_rf: Node    = n.rotation_rf()
	var data_children_translation: Dictionary = serialize_children( translation_rf, [rotation_rf] )
	var data_children_rotation: Dictionary    = serialize_children( rotation_rf, [] )
	var data: Dictionary = {
		filename = filename, 
		data     = data_self, 
		children_translation = data_children_translation, 
		children = data_children_rotation
	}
	print( "serialization for \'", n.name, "\' is done" )
	return data




static func serialize_children( n: Node, exclusion: Array ):
	print( "started serializing children for \'", n.name, "\'" )
	var children_data: Dictionary = {}
	var qty: int = n.get_child_count()
	print( "number of children to serialize: ", qty )
	for i in range( qty ):
		print( "serializing child #: ", i )
		var ch: Node = n.get_child( i )
		if ch in exclusion:
			continue
		print( "child name: \'", ch.name, "\'" )
		var data: Dictionary = serialize( ch )
		var name: String = ch.name
		children_data[name] = data
	
	return children_data




static func deserialize( node: Node, data: Dictionary ):
	var cl: String = node.get_class()
	var ret: bool
	if cl == "CelestialSurface":
		ret = deserialize_celestial_surface( node, data )
	else:
		ret = deserialize_common( node, data )
	
	return ret



static func deserialize_common( node: Node, data: Dictionary ):
	var has_data: bool = data.has( "data" )
	var has_deserialize: bool = node.has_method( "deserialize" )
	if has_data and has_deserialize:
		var node_data: Dictionary = data["data"]
		print( "deserializing \'", node.name, "\' ..." )
		var ret = node.deserialize( node_data )
		print( "deserialization result for \'", node.name, "\': ", ret )
	var has_children: bool = data.has( "children" )
	if has_children:
		var data_children: Dictionary = data["children"]
		print( "deserializing children of \'", node.name, "\' ..." )
		var ret = deserialize_children( node, data_children )
		print( "deserialization result for children of \'", node.name, "\': ", ret )
		return ret
	else:
		print( "no children to serialize for node \'", node.name, "\'" )
	return true


static func deserialize_celestial_surface( node: Node, data: Dictionary ):
	var has_data: bool = data.has( "data" )
	var has_deserialize: bool = node.has_method( "deserialize" )
	if has_data and has_deserialize:
		var node_data: Dictionary = data["data"]
		print( "deserializing \'", node.name, "\' ..." )
		var ret = node.deserialize( node_data )
		print( "deserialization result for \'", node.name, "\': ", ret )
	var has_children: bool = data.has( "children" )
	if has_children:
		var data_children: Dictionary = data["children"]
		print( "deserializing children of \'", node.name, "\' ..." )
		var ret = deserialize_children( node, data_children )
		print( "deserialization result for children of \'", node.name, "\': ", ret )
		return ret
	else:
		print( "no children to serialize for node \'", node.name, "\'" )
	return true




static func deserialize_children( parent: Node, data: Dictionary ):
	print( "creating children for \'", parent.name, "\'" )
	var has_children: bool = data.has( "children" )
	print( "has_children: ", has_children )
	var children_data: Dictionary = data["children"]
	for name in data:
		print( "trying to create \'", name, "\'" )
		var child_data_full: Dictionary = data[name]
		# We must have "node" entry and optinally "children" entry.
		var has_filename: bool = child_data_full.has( "filename" )
		print( "has filename: ", has_filename )
		if has_filename :
			var filename: String = child_data_full["filename"]
			var R: Resource = load( filename )
			var n: Node = R.instance()
			n.name = name
			print( "created the instance successfully" )
			parent.add_child( n )
			print( "deserializing \'", name, "\'" )
			var ret = deserialize( n, child_data_full )
			print( "deserialization result for \'", name, "\': ", ret )
	


