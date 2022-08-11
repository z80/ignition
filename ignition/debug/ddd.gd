
extends Node

## @brief Color of the text drawn as HUD
const TEXT_COLOR: Color           = Color(0.0, 0.3, 0.0 )
const TEXT_COLOR_IMPORTANT: Color = Color(0.5, 0.0, 0.0 )
const TEXT_COLOR_PERMANENT: Color = Color(0.58, 0.0, 0.64)
## @brief Background color of the text drawn as HUD
const TEXT_BG_COLOR = Color(0.3, 0.3, 0.3, 0.2)

var _canvas_item : CanvasItem = null
var _texts: Array = []
var _font : Font = null
var _color: Color = TEXT_COLOR

export(bool) var debug = true

func _ready():
	# Get default font
	# Meh
	var c := Control.new()
	add_child(c)
	_font = c.get_font( "font" )
	c.queue_free()
	
	_canvas_item = Node2D.new()
	_canvas_item.position = Vector2(8, 8)
	_canvas_item.connect( "draw", self, "_on_CanvasItem_draw" )
	_canvas_item.z_index = 100

	add_child(_canvas_item)




func important():
	_color = TEXT_COLOR_IMPORTANT


func print( stri: String, timeout: float = 5.0, key: String = "" ):
	if not debug:
		return
	
	if not key.empty():
		var qty: int = _texts.size()
		for i in range(qty):
			var t: Dictionary = _texts[i]
			var k: String = t.key
			if k == key:
				t.stri  = stri
				t.color = _color
				return
	
	_texts.push_back( {
		"stri": stri,
		"timeout": timeout, 
		"key": key, 
		"color": _color
	} )
	
	_color = TEXT_COLOR


func _process(delta: float):
	var qty: int = _texts.size()
	var new_texts: Array = []
	# Remove text lines after some time
	for i in range(qty):
		var t: Dictionary = _texts[i]
		var timeout: float = t.timeout
		var draw: bool
		if timeout >= 0.0:
			timeout -= delta
			if timeout >= 0.0:
				t.timeout = timeout
				new_texts.push_back( t )
		
		else:
			new_texts.push_back( t )
		
	_texts = new_texts

	# Update canvas
	_canvas_item.update()


func _on_CanvasItem_draw():
	var ci: Node2D = _canvas_item

	var ascent: Vector2 = Vector2(0, _font.get_ascent())
	var pos: Vector2    = Vector2.ZERO
	var xpad: int = 2
	var ypad: int = 1
	var font_offset: Vector2 = ascent + Vector2(xpad, ypad)
	var line_height: int = _font.get_height() + 2 * ypad

	var qty: int = _texts.size()
	for i in range(qty):
		var t: Dictionary = _texts[i]
		var key: String = t.key
		
		if not key.empty():
			var text: String = t.stri
			var ss := _font.get_string_size(text)
			#ci.draw_rect( Rect2(pos, Vector2(ss.x + xpad * 2, line_height)), TEXT_BG_COLOR )
			ci.draw_string( _font, pos + font_offset, text, TEXT_COLOR_IMPORTANT )
			pos.y += line_height

	for i in range(qty):
		var t: Dictionary = _texts[i]
		var key: String = t.key
		
		if key.empty():
			var text: String = t.stri
			var color: Color = t.color
			var ss := _font.get_string_size(text)
			#ci.draw_rect( Rect2(pos, Vector2(ss.x + xpad * 2, line_height)), TEXT_BG_COLOR )
			ci.draw_string( _font, pos + font_offset, text, color )
			pos.y += line_height



