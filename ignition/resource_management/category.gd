
extends Resource
class_name CreatableCategory


export(Resource) var icon = null
export(String)   var category = ""
export(String)   var category_visual = "Human readable name"
# If blocks existing in this category are static bodies.
export(bool)     var is_static = false
# This one is for sorting. Categories with smaller index go first.
export(int)      var index = 0
