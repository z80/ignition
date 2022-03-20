

static func find_descs( path: String ):
	var ret: Array = []
	_find_descs_recursive( path, ret )
	return ret

static func find_techs( path: String ):
	var ret: Array = []
	_find_techs_recursive( path, ret )
	return ret


static func find_categories( path: String ):
	var ret: Array = []
	_find_categories_recursive( path, ret )
	return ret



static func _find_descs_recursive( path: String, ret: Array ):
	var dir: Directory = Directory.new()
	var ok: bool = (dir.open( path ) == OK)
	if not ok:
		return
	
	dir.list_dir_begin()
	var file_name: String = dir.get_next()
	
	while (file_name != ""):
		
		if (file_name != ".") and (file_name != ".."):
			
			if dir.current_is_dir():
				#print("Found directory: " + file_name)
				var new_path: String = path
				if not path.ends_with( '/' ):
					new_path = new_path + "/"
				new_path = new_path + file_name
				_find_descs_recursive( new_path, ret )
			
			else:
				#print("Found file: " + file_name)
				var is_desc: bool = _is_desc( file_name )
				if is_desc:
					var full_path: String = path
					if not path.ends_with( '/' ):
						full_path = full_path + "/"
					full_path = full_path + file_name
					var res: Resource = load( full_path )
					ret.push_back( res )
			
		file_name = dir.get_next()


static func _find_techs_recursive( path: String, ret: Array ):
	var dir: Directory = Directory.new()
	var ok: bool = (dir.open( path ) == OK)
	if not ok:
		return
	
	dir.list_dir_begin()
	var file_name: String = dir.get_next()
	
	while (file_name != ""):
		
		if (file_name != ".") and (file_name != ".."):
			
			if dir.current_is_dir():
				#print("Found directory: " + file_name)
				var new_path: String = path
				if not path.ends_with( '/' ):
					new_path = new_path + "/"
				new_path = new_path + file_name
				_find_techs_recursive( new_path, ret )
			
			else:
				#print("Found file: " + file_name)
				var is_desc: bool = _is_tech( file_name )
				if is_desc:
					var full_path: String = path
					if not path.ends_with( '/' ):
						full_path = full_path + "/"
					full_path = full_path + file_name
					var res: Resource = load( full_path )
					ret.push_back( res )
			
		file_name = dir.get_next()


static func _find_categories_recursive( path: String, ret: Array ):
	var dir: Directory = Directory.new()
	var ok: bool = (dir.open( path ) == OK)
	if not ok:
		return
	
	dir.list_dir_begin()
	var file_name: String = dir.get_next()
	
	while (file_name != ""):
		
		if (file_name != ".") and (file_name != ".."):
			
			if dir.current_is_dir():
				#print("Found directory: " + file_name)
				var new_path: String = path
				if not path.ends_with( '/' ):
					new_path = new_path + "/"
				new_path = new_path + file_name
				_find_categories_recursive( new_path, ret )
			
			else:
				#print("Found file: " + file_name)
				var is_desc: bool = _is_category( file_name )
				if is_desc:
					var full_path: String = path
					if not path.ends_with( '/' ):
						full_path = full_path + "/"
					full_path = full_path + file_name
					var res: Resource = load( full_path )
					ret.push_back( res )
			
		file_name = dir.get_next()

static func _is_desc( file_name: String ):
	var ret: bool = (file_name == "desc.tres") or (file_name == "desc.res")
	return ret


static func _is_tech( file_name: String ):
	var ret: bool = (file_name == "tech.tres") or (file_name == "tech.res")
	return ret


static func _is_category( file_name: String ):
	var ret: bool = (file_name == "category.tres") or (file_name == "category.res")
	return ret




static func get_package_files():
	var path: String = OS.get_executable_path()



static func load_package_files():
	var path: String = 'res://'
	var dir: Directory = Directory.new()
	var ok: bool = (dir.open( path ) == OK)
	if not ok:
		return
	
	dir.list_dir_begin()
	var file_name: String = dir.get_next()
	
	while (file_name != ""):
		var is_pck: bool = file_name.ends_with( "pck" )
		if not is_pck:
			continue
		
		var new_path: String = path
		if not path.ends_with( '/' ):
			new_path = new_path + "/"
		new_path = new_path + file_name

		ProjectSettings.load_resource_pack( new_path )

