extends Control

signal wiki_page( path )

const SUFFIX: String = ".wiki"

@export var root_folder: String = "res://wiki"
@export var root_file: String   = "root"
@export var start_file: String  = "root"


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
	var full_path: String = root_folder + '/' + path + SUFFIX
	open_internal( full_path, path, update_history )


# For opening files not under "res://wiki" path.
func open_global( full_path: String, title_path: String ):
	open_internal( full_path, title_path )


func open_internal( full_path: String, path: String, update_history: bool = true ):
	var file: FileAccess = FileAccess.open( full_path, FileAccess.READ )
	if file != null:
		return
	
	var text: String = file.get_as_text()
	
	var l: RichTextLabel = _get_text_area()
	l.text = text
	l.scroll_to_line( 0 )
	emit_signal( "wiki_page", path )
	
	if update_history:
		_update_history( full_path, path )


func _update_history( full_path: String, path: String ):
	var qty: int = _history.size()
	var entry: Array = [ full_path, path ]
	if (_history_index < 0) or (_history_index >= qty):
		_history.push_back( entry )
		_history_index = qty
	
	else:
		_history_index += 1
		_history.insert( _history_index, entry )



func _go_home():
	open( root_file )


func _go_back():
	var qty: int = _history.size()
	if (qty > 0) and (_history_index > 0):
		_history_index -= 1
		var entry: Array = _history[_history_index]
		open_internal( entry[0], entry[1], false )


func _go_forward():
	var last_index: int = _history.size() - 1
	if _history_index < last_index:
		_history_index += 1
		var entry: Array = _history[_history_index]
		open_internal( entry[0], entry[1], false )




func _on_meta_clicked( meta ):
	var path: String = str( meta )
	var is_http: bool = is_http_url( path )
	
	if is_http:
		var _err_code: int = OS.shell_open( path )
	
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
	var _err_code: int = re.compile( '^https?://.+' )
	var result = re.search( path )
	var ret: bool = (result != null)
	
	return ret


