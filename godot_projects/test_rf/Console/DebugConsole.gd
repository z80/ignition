extends CanvasLayer

onready var console = $Consoles/Console


func _ready():
	pass


func write_channel(channelName, text, flags = 0):
	console.write_channel(channelName, text, flags)
	
	
func write_line_channel(channelName, text, flags = 0):
	console.write_line_channel(channelName, text + "\n", flags)


func write(msg, flags = 0):
	console.write_channel("All", msg, flags)


func write_line(msg, flags = 0):
	console.write_line_channel("All", msg, flags)


func error(msg, channelName = "All"):
	if channelName.empty():
		console.error(msg, 0)
	else:
		console.error(msg, 0, console.get_channel(channelName))


func warn(msg, channelName = "All"):
	if channelName.empty():
		console.warn(msg, 0)
	else:
		console.warn(msg, 0, console.get_channel(channelName))


func success(msg, channelName = "All"):
	if channelName.empty():
		console.success(msg, 0)
	else:
		console.success(msg, 0, console.get_channel(channelName))


func add_command(command : ConsoleCommand):
	console.add_command(command)


func get_channels() -> Array:
	return console.get_channels()


func get_channel(channelName : String) -> ConsoleChannel:
	return console.get_channel(channelName)


func add_channel(channelName : String):
	console.add_channel(channelName)


func remove_channel(channelName : String):
	console.remove_channel(channelName)





