
var _utils = load('res://addons/gut/utils.gd').get_instance()
# Hash containing all the built in types in Godot.  This provides an English
# name for the types that corosponds with the type constants defined in the
# engine.
var types = {}

func _init_types_dictionary():
	types[TYPE_NIL] = 'TYPE_NIL'
	types[TYPE_BOOL] = 'Bool'
	types[TYPE_INT] = 'Int'
	types[TYPE_FLOAT] = 'Float/Real'
	types[TYPE_STRING] = 'String'
	types[TYPE_VECTOR2] = 'Vector2'
	types[TYPE_RECT2] = 'Rect2'
	types[TYPE_VECTOR3] = 'Vector3'
	#types[8] = 'Matrix32'
	types[TYPE_PLANE] = 'Plane'
	types[TYPE_QUATERNION] = 'QUAT'
	types[TYPE_AABB] = 'AABB'
	#types[12] = 'Matrix3'
	types[TYPE_TRANSFORM3D] = 'Transform3D'
	types[TYPE_COLOR] = 'Color'
	#types[15] = 'Image'
	types[TYPE_NODE_PATH] = 'Node Path3D'
	types[TYPE_RID] = 'RID'
	types[TYPE_OBJECT] = 'TYPE_OBJECT'
	#types[19] = 'TYPE_INPUT_EVENT'
	types[TYPE_DICTIONARY] = 'Dictionary'
	types[TYPE_ARRAY] = 'Array'
	types[TYPE_PACKED_BYTE_ARRAY] = 'TYPE_PACKED_BYTE_ARRAY'
	types[TYPE_PACKED_INT32_ARRAY] = 'TYPE_PACKED_INT32_ARRAY'
	types[TYPE_PACKED_FLOAT32_ARRAY] = 'TYPE_PACKED_FLOAT32_ARRAY'
	types[TYPE_PACKED_STRING_ARRAY] = 'TYPE_PACKED_STRING_ARRAY'
	types[TYPE_PACKED_VECTOR2_ARRAY] = 'TYPE_PACKED_VECTOR2_ARRAY'
	types[TYPE_PACKED_VECTOR3_ARRAY] = 'TYPE_PACKED_VECTOR3_ARRAY'
	types[TYPE_PACKED_COLOR_ARRAY] = 'TYPE_PACKED_COLOR_ARRAY'
	types[TYPE_MAX] = 'TYPE_MAX'

# Types to not be formatted when using _str
var _str_ignore_types = [
	TYPE_INT, TYPE_FLOAT, TYPE_STRING,
	TYPE_NIL, TYPE_BOOL
]

func _init():
	_init_types_dictionary()

# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
func _get_filename(path):
	return path.split('/')[-1]

# ------------------------------------------------------------------------------
# Gets the filename of an object passed in.  This does not return the
# full path to the object, just the filename.
# ------------------------------------------------------------------------------
func _get_obj_filename(thing):
	var filename = null

	if(thing == null or
		!is_instance_valid(thing) or
		str(thing) == '[Object:null]' or
		typeof(thing) != TYPE_OBJECT or
		thing.has_method('__gut_instance_from_id')):
		return

	if(thing.get_script() == null):
		if(thing is PackedScene):
			filename = _get_filename(thing.resource_path)
		else:
			# If it isn't a packed scene and it doesn't have a script then
			# we do nothing.  This just read better.
			pass
	elif(!_utils.is_native_class(thing)):
		var dict = inst_to_dict(thing)
		filename = _get_filename(dict['@path'])
		if(dict['@subpath'] != ''):
			filename += str('/', dict['@subpath'])

	return filename

# ------------------------------------------------------------------------------
# Better object/thing to string conversion.  Includes extra details about
# whatever is passed in when it can/should.
# ------------------------------------------------------------------------------
func type2str(thing):
	var filename = _get_obj_filename(thing)
	var str_thing = str(thing)

	if(thing == null):
		# According to str there is a difference between null and an Object
		# that is somehow null.  To avoid getting '[Object:null]' as output
		# always set it to str(null) instead of str(thing).  A null object
		# will pass typeof(thing) == TYPE_OBJECT check so this has to be
		# before that.
		str_thing = str(null)
	elif(typeof(thing) in _str_ignore_types):
		# do nothing b/c we already have str(thing) in
		# to_return.  I think this just reads a little
		# better this way.
		pass
	elif(typeof(thing) ==  TYPE_OBJECT):
		if(_utils.is_native_class(thing)):
			str_thing = _utils.get_native_class_name(thing)
		elif(_utils.is_double(thing)):
			var double_path = _get_filename(thing.__gut_metadata_.path)
			if(thing.__gut_metadata_.subpath != ''):
				double_path += str('/', thing.__gut_metadata_.subpath)

			str_thing += '(double of ' + double_path + ')'
			filename = null
	elif(types.has(typeof(thing))):
		if(!str_thing.begins_with('(')):
			str_thing = '(' + str_thing + ')'
		str_thing = str(types[typeof(thing)], str_thing)

	if(filename != null):
		str_thing += str('(', filename, ')')
	return str_thing
