

static func find_blocks( path: String ):
	var ret: Array = []
	_find_blocks_recursive( path, ret )
	return ret

static func find_techs( path: String ):
	var ret: Array = []
	_find_techs_recursive( path, ret )
	return ret


static func find_categories( path: String ):
	var ret: Array = []
	_find_categories_recursive( path, ret )
	return ret



static func _find_blocks_recursive( path: String, ret: Array ):
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
				_find_blocks_recursive( new_path, ret )
			
			else:
				#print("Found file: " + file_name)
				var is_desc: bool = _is_block( file_name )
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

static func _is_block( file_name: String ):
	var ret: bool = (file_name == "block.tres") or (file_name == "block.res")
	return ret


static func _is_tech( file_name: String ):
	var ret: bool = (file_name == "tech.tres") or (file_name == "tech.res")
	return ret


static func _is_category( file_name: String ):
	var ret: bool = (file_name == "category.tres") or (file_name == "category.res")
	return ret




static func get_package_files():
	var ret: Array = []
	var path: String = OS.get_executable_path()
	var re: RegEx = RegEx.new()
	var compile_result: int = re.compile( '\/([^\/]+)$' )
	if compile_result != OK:
		return ret
	
	var res: RegExMatch = re.search( path )
	var fname: String   = res.strings[1]
	var has_exe: bool   = fname.to_lower().ends_with( ".exe" )
	if has_exe:
		fname = fname.substr( 0, fname.length() - 4 )
	fname = fname + ".pck"
	
	var root_path: String = 'res://'
	var dir: Directory = Directory.new()
	var ok: bool = (dir.open( root_path ) == OK)
	if not ok:
		return ret
	
	dir.list_dir_begin()
	var file_name: String = dir.get_next()
	
	while (file_name != ""):
		var is_pck: bool = file_name.ends_with( ".pck" )
		if not is_pck:
			file_name = dir.get_next()
			continue
		
		if file_name.ends_with( fname ):
			file_name = dir.get_next()
			continue
		
		var new_path: String = path
		if not path.ends_with( '/' ):
			new_path = new_path + "/"
		new_path = new_path + file_name

		ret.push_back( new_path )
		
		file_name = dir.get_next()
	
	return ret


static func load_package_files():
	var files: Array = get_package_files()
	
	for file in files:
		ProjectSettings.load_resource_pack( file )

