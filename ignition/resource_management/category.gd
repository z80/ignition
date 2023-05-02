
extends Resource
class_name CreatableCategory


@export var icon: Resource = null
@export var category: String = ""
@export var category_visual: String = "Human readable name"
# If blocks existing in this category are static bodies.
@export var is_static: bool = false
# This one is for sorting. Categories with smaller index go first.
@export var index: int = 0
