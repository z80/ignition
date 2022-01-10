extends Control

signal wiki_page( path )

const SUFFIX: String = ".wiki"

export(String) var root_folder = "res://wiki"
export(String) var root_file   = "root"
export(String) var start_file  = "root"


var _text_area: RichTextLabel = null
var _back: Button             = null
var _forward: Button          = null

var _history: Array = []
var _history_index: int = -1

# Called when the node enters the scene tree for the first time.
func _ready():
	open( start_file )


func open( path: String, update_history: bool = true ):
	path = clean_path( path )
	var file_name: String = root_folder + '/' + path + SUFFIX
	var file: File = File.new()
	var ret: int = file.open( file_name, File.READ )
	if ret != OK:
		return
	
	var text: String = file.get_as_text()
	
	var l: RichTextLabel = _get_text_area()
	l.bbcode_text = text
	l.scroll_to_line( 0 )
	emit_signal( "wiki_page", path )
	
	if update_history:
		_update_history( path )



func _update_history( path: String ):
	var qty: int = _history.size()
	if (_history_index < 0) or (_history_index >= qty):
		_history.push_back( path )
		_history_index = qty
	
	else:
		_history_index += 1
		_history.insert( _history_index, path )



func _go_home():
	open( root_file )


func _go_back():
	var qty: int = _history.size()
	if (qty > 0) and (_history_index > 0):
		_history_index -= 1
		var path: String = _history[_history_index]
		open( path, false )


func _go_forward():
	var last_index: int = _history.size() - 1
	if _history_index < last_index:
		_history_index += 1
		var path: String = _history[_history_index]
		open( path, false )




func _on_meta_clicked( meta ):
	var path: String = str( meta )
	var is_http: bool = is_http_url( path )
	
	if is_http:
		OS.shell_open( path )
	
	else:
		open( path )



func _get_text_area():
	if _text_area == null:
		_text_area = get_node( "TextArea" )
	
	return _text_area



func _get_back():
	if _back == null:
		_back = get_node( "Buttons/Back" )
	
	return _back



func _get_forward():
	if _forward == null:
		_forward = get_node( "Buttons/Forward" )
	
	return _forward



static func clean_path( path: String ):
	return path



static func is_http_url( path: String ):
	var re: RegEx = RegEx.new()
	re.compile( '^https?:\/\/.+' )
	var result = re.search( path )
	var ret: bool = (result != null)
	
	return ret


