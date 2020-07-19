class_name ConsoleFlags

enum Type {
	NONE = 0,
	USER_PREFIX = 1,
	CLICKABLE = 2048,
	NO_DRAW = 4, # won't be drawn on console text
	BOLD = 8,
	ITALICS = 16,
	UNDERLINE = 32,
	CODE = 64,
	CENTER = 128,
	RIGHT = 256,
	FILL = 512,
	INDENT = 1024,
	URL = 2048,
	NO_COMMANDS = 4096,
	TRACK_MESSAGE = 8192, # append to the message stack that can be scrolled through with ui_up/ui_down 
	SHOW_TIME = 16384,
}
