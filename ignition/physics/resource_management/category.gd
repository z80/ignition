
extends Resource
class_name CreatableCategory


export(Resource) var icon = null
export(String)   var category = ""
export(String)   var category_visual = "Human readable name"
# This one is for sorting. Categories with smaller index go first.
export(int)      var index = 0
