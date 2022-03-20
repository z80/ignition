
extends Node


export(bool) var all_available = true


var _block_descs: Array      = []
var _categories: Array       = []
var _techs_all: Array        = []
# Here are just names.
var _techs_accessible: Array = []



func get_categories():
	return _categories



func get_blocks( category: Resource ):
	var ret: Array = []
	var category_name: String = category.category
	for desc in _block_descs:
		var desc_category: String = desc.category
		if desc_category != category_name:
			continue
		
		var required_tech: String = desc.required_tech
		var allowed: bool = _is_accessible( desc )
		ret.push_back( desc )
	
	return ret





# Called when the node enters the scene tree for the first time.
func _ready():
	_parse_resources()


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass







func _is_accessible( desc: Resource ):
	if all_available:
		return true
	
	var required_tech: String = desc.required_tech
	for tech in _techs_accessible:
		var tech_name: String = tech.tech_name
		if required_tech == tech_name:
			return true
	
	return false



func _parse_resources():
	var Finder   = preload( "res://resource_management/resource_finder.gd" )
	
	# First load all package files.
	Finder.load_package_files()
	
	_block_descs = Finder.find_blocks( 'res://' )
	_categories  = Finder.find_categories( 'res://' )
	_techs_all   = Finder.find_techs( 'res://' )
	
	_sort_categories()
	#print( "Block descs: ", _block_descs )


func _sort_categories():
	var qty: int = _categories.size()
	for i in range(qty-1):
		var cat_a: Resource = _categories[i]
		for j in range(1, qty):
			var cat_b: Resource = _categories[j]
			if cat_b.index < cat_a.index:
				_categories[i] = cat_b
				_categories[j] = cat_a
				cat_a = cat_b
				continue
			
			elif (cat_b.index == cat_a.index) and (cat_b.category < cat_a.category):
				_categories[i] = cat_b
				_categories[j] = cat_a
				cat_a = cat_b
				continue




