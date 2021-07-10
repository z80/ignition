

static func serialize( n: Node ):
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
	var data_children: Dictionary  = serialize_children( n )
	var data: Dictionary = {
		filename = filename, 
		data     = data_self, 
		children = data_children
	}
	print( "serialization for \'", n.name, "\' is done" )
	return data



static func serialize_children( n: Node ):
	print( "started serializing children for \'", n.name, "\'" )
	var children_data: Dictionary = {}
	var qty: int = n.get_child_count()
	print( "number of children to serialize: ", qty )
	for i in range( qty ):
		print( "serializing child #: ", i )
		var ch: Node = n.get_child( i )
		print( "child name: \'", ch.name, "\'" )
		var data: Dictionary = serialize( ch )
		var name: String = ch.name
		children_data[name] = data
	
	return children_data



static func deserialize( node: Node, data: Dictionary ):
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
	


