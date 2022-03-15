
extends Node


export(bool) var all_available = true


var _descs: Array            = []
var _categories: Array       = []
var _techs_all: Array        = []
# Here are just names.
var _techs_accessible: Array = []



func get_categories():
	return _categories



func get_parts( category: Resource ):
	var ret: Array = []
	var category_name: String = category.category
	for desc in _descs:
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
	var Finder  = preload( "res://physics/resource_management/resource_finder.gd" )
	_descs      = Finder.find_descs( 'res://' )
	_categories = Finder.find_categories( 'res://' )
	_techs_all  = Finder.find_techs( 'res://' )
	
	_sort_categories()
	print( "descs: ", _descs )


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
			
			if cat_b.name < cat_a.name:
				_categories[i] = cat_b
				_categories[j] = cat_a
				cat_a = cat_b
				continue




