
extends Node
class_name RluCache

export(int) var size = 1024
var queue: Array = []
var dict: Dictionary = {}


func query( key ):
	var exists = dict.has( key )
	if not exists:
		return null
	var p = dict[key]
	var v = p[1]
	return v

func insert( key, value ):
	# Limit the size
	var sz = queue.size()
	while sz >= size:
		var n = queue.back()
		var k = n[0]
		dict.erase( k )
		queue.pop_back()
	# Also make sure the same key doesn't exist
	var exists = dict.has( key )
	if exists:
		var p = dict[key]
		queue.erase( p )
		dict.erase( key )
	# Insert new value
	var v = [ key, value ]
	queue.push_front( v )
	dict[key] = v


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


