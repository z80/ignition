extends Control
class_name Console
tool

# SIGNALS

signal on_message_sent(msg)
signal on_command_sent(msg)


# CONSTS

# only shown in editor
const previewLineMessage := "%senter text here..."
const previewConsoleText := "%s%sman help"

# Keys
const nextMessageKey := "ui_down"
const previousMessageKey := "ui_up"
const autoCompleteKey := "ui_focus_next"
const toggleConsoleKey := KEY_QUOTELEFT

# makrso for predefined messages
const COMMAND_NOT_FOUND_MSG := "[color=red]Command not found![/color]"
const WARN_MSG := "[color=yellow]%s[/color]"
const WARN_MSG_PREFIX := " [WARNING] "
const ERROR_MSG := "[color=red]%s[/color]"
const ERROR_MSG_PREFIX := " [ERROR] "
const SUCCESSFUL_MSG := "[color=green]%s[/color]"
const SUCCESSFUL_MSG_PREFIX := " [SUCCESS] "


# EXPORT VARS

export(String) var userName = "" setget update_user_name
export(ConsoleUserRight.Type) var userRight = ConsoleUserRight.Type.DEV
export(ConsoleColor.Type) var consoleUserNameColor setget update_console_user_name_color

export(ConsoleTheme.Type) var design setget update_design
export(Dockings.Type) var dockingStation = Dockings.Type.TOP setget update_docking_station
export(ConsoleColor.Type) var textColor = ConsoleColor.Type.WHITE setget update_text_color

# CHANNELS
export(Array, String) var channelNames = ["All"]

# OTHER VISIUALS
export(bool) var hideScrollBar = false setget update_show_scroll_bar
export(bool) var roundedTitleBarEdges = true setget update_rounded_title_bar

# SHOW/HIDE
export(bool) var showTitleBar = true setget update_show_title_bar
export(bool) var showTextBackground = true setget update_show_text_background
export(bool) var showConsoleBackground = true setget update_show_console_background
export(bool) var showLineBackground = false setget update_show_line_background
export(bool) var showSendButton = true setget update_show_send_button
export(bool) var showChannelButton = true setget update_show_channel_button

# COLORS
export(Color) var titleBarBackgroundColor setget update_title_bar_color
export(Color) var backgroundColor setget update_background_color
export(Color) var textBackgroundColor setget update_text_background_color
export(Color) var lineBackgroundColor setget update_line_background_color
export(Color) var sendButtonColor setget update_send_button_color
export(Color) var sendButtonTextColor setget update_send_button_text_color

# MISC
export(bool) var allowWindowDrag = true
export(String, "default") var slideInAnimation = "default"

# SIGNS and other text related things
export(bool) var showTimeStamp = false setget update_show_time_stamp
export(String) var userMsgSign = ">" setget update_user_message_sign
export(String) var commandSign = "/" setget update_user_command_sign
export(bool) var sendUserName = false setget update_send_user_name

# LOG
export(bool) var logEnabled = false
export(float) var logInterval = 300.0 setget update_log_time_interval



# ONREADY VARS 

onready var consoleLine := $Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/Input/Line
onready var consoleText := $Control/VBoxContainer/ConsoleContainer/ConsoleText/RichTextLabel

# for background colors
#onready var background := $Control/VBoxContainer/ConsoleContainer/ConsoleBackground
#onready var TitleBarBackground := $Control/VBoxContainer/TitleBar/TitleBarBackground
#onready var textBackground := $Control/VBoxContainer/ConsoleContainer/ConsoleText/TextBackground
#onready var lineBackground := $Control/VBoxContainer/ConsoleContainer/InputLine/LineBackground
#onready var buttonBackground := $Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ButtonField/ButtonBackground

#onready var sendButton := $Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ButtonField/SendText
#onready var hideConsoleButton := $Control/VBoxContainer/TitleBar/VBoxContainer/HideConsole
onready var channelsButton := $Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ChannelSelector/Channels

onready var logTimer = $LogTimer


# VARS

var completeText := ""

var channelTexts := []

var allChannel : ConsoleChannel
var selectedChannel : ConsoleChannel

var channelChanged := false

var messageIndex := 0
var enteredMessages := []
var consoleTextColor : Color
var prefix := ""
var postfix := ""

var commands := []
var basicCommandsAmount := 0

var userNameColor : Color

var startWindowDragPos : Vector2
var dragging : bool
var defaultConsoleSize := Vector2(550.0, 275.0)

var logFile := File.new()
var logFileCreated := false
var logFileName := "log.txt"
var logFilePath := ""
var logType := "" # e.g. [ERROR]/[WARNING]/[SUCCESS]

var showPreview = true



func _ready():
	showPreview = false
	
	consoleText.bbcode_text = ""
	
	consoleLine.placeholder_text = userMsgSign
	consoleLine.text = ""
	
	add_basic_commands()
	basicCommandsAmount = commands.size()
	
	for i in channelsButton.get_item_count():
		remove_channel(channelsButton.get_item_text(0))
	
	allChannel = add_channel("All")
	selectedChannel = allChannel
	
	for channelName in channelNames:
		if channelName != "All":
			var _c = add_channel(channelName)
	
	if logEnabled:
		update_log_time_interval(logInterval)
		create_log_file()
		logTimer.start()


func add_channel(channelName : String) -> ConsoleChannel:
	channelsButton.add_item(channelName)
	var newChannel = ConsoleChannel.new(channelName)
	channelTexts.append(newChannel)
	return newChannel


func remove_channel(channelName : String):
	channelTexts.erase(channelName)
	for _idx in range(channelsButton.get_item_count()):
		if channelsButton.get_item_text(0) == channelName:
			channelsButton.remove_item(0)


func get_channels() -> Array:
	return channelTexts


func _notification(what):
	if what == MainLoop.NOTIFICATION_WM_QUIT_REQUEST:
		if logEnabled:
			_on_LogTimer_timeout()


func add_basic_commands():
	var defArgs = ConsoleDefaultCommands.new()
	defArgs.set_default_commands(self)
	add_child(defArgs)


func toggle_visibility():
	visible = ! visible


func toggle_console():
	if slideInAnimation == "default":
		slideInAnimation = ""
	if visible:
		$Animation.play("slide_out%s" % slideInAnimation)
	else:
		$Animation.play("slide_in%s" % slideInAnimation)


func apply_flags(flags):
	postfix = ""
	prefix = ""
	
	if (flags & ConsoleFlags.Type.BOLD) == ConsoleFlags.Type.BOLD:
		prefix += "[b]"
		postfix = postfix.insert(0, "[/b]")
	if (flags & ConsoleFlags.Type.ITALICS) == ConsoleFlags.Type.ITALICS:
		prefix += "[i]"
		postfix = postfix.insert(0, "[/i]")
	if (flags & ConsoleFlags.Type.UNDERLINE) == ConsoleFlags.Type.UNDERLINE:
		prefix += "[u]"
		postfix = postfix.insert(0, "[/u]")
	if (flags & ConsoleFlags.Type.CODE) == ConsoleFlags.Type.CODE:
		prefix += "[code]"
		postfix = postfix.insert(0, "[/code]")
	if (flags & ConsoleFlags.Type.CENTER) == ConsoleFlags.Type.CENTER:
		prefix += "[center]"
		postfix = postfix.insert(0, "[/center]")
	if (flags & ConsoleFlags.Type.RIGHT) == ConsoleFlags.Type.RIGHT:
		prefix += "[right]"
		postfix = postfix.insert(0, "[/right]")
	if (flags & ConsoleFlags.Type.FILL) == ConsoleFlags.Type.FILL:
		prefix += "[fill]"
		postfix = postfix.insert(0, "[/fill]")
	if (flags & ConsoleFlags.Type.INDENT) == ConsoleFlags.Type.INDENT:
		prefix += "[indent]"
		postfix = postfix.insert(0, "[/indent]")
	if (flags & ConsoleFlags.Type.URL) == ConsoleFlags.Type.URL:
		prefix += "[url]"
		postfix = postfix.insert(0, "[/url]")


func success(msg, flags = 0, channel = selectedChannel):
	write_line(SUCCESSFUL_MSG % msg, flags, channel)


func warn(msg, flags = 0, channel = selectedChannel):
	write_line(WARN_MSG % msg, flags, channel)


func error(msg, flags = 0, channel = selectedChannel):
	write_line(ERROR_MSG % msg, flags, channel)


func write_line(msg, flags = 0, channel = selectedChannel):
	if !msg.empty():
		write(msg, flags, channel)
	
	write("\n", 0, channel)


func clear_selected_channel_text():
	selectedChannel.text = ""
	consoleText.bbcode_text = ""


func _clear_channel_cache(channel : ConsoleChannel):
	channel.lastMessage = ""


func write(msg, flags = 0, channel = selectedChannel):
	if msg.empty():
		return
	apply_flags(flags)
	
	if !flags & ConsoleFlags.Type.NO_DRAW:
		if !channel.lastMessage.empty():
			channel.lastMessage = ""
			allChannel.lastMessage = ""
		
		if flags & ConsoleFlags.Type.SHOW_TIME:
			_write_to_selected_channel(channel, get_time_stamp(false))
		
		if flags & ConsoleFlags.Type.USER_PREFIX:
			_write_to_selected_channel(channel, \
					str(userMsgSign, " ", color_to_bb_code(userName, userNameColor), ": "))
		
		if prefix.length() > 0:
			_write_to_selected_channel(channel, prefix)
		
		if flags & ConsoleFlags.Type.TRACK_MESSAGE:
			enteredMessages.append(msg)
			messageIndex = -1
		
		_write_to_selected_channel(channel, msg)
		
		if postfix.length() > 0:
			_write_to_selected_channel(channel, postfix)
		
		# if the channel changed, then change all text
		_apply_channel_text(channel)
	
	if flags & ConsoleFlags.Type.CLICKABLE:
		consoleText.push_meta(msg)
	
	if msg[0] == commandSign and !flags & ConsoleFlags.Type.NO_COMMANDS: # check if the input is a command
		execute_command(msg)
		
	if logEnabled:
		if flags & ConsoleFlags.Type.USER_PREFIX and sendUserName:
			msg = str(userMsgSign, " ", color_to_bb_code(userName, userNameColor), ": ") + msg
		add_to_log(msg) 
		
	
	emit_signal("on_message_sent", msg)


func _write_to_selected_channel(channel : ConsoleChannel, text : String):
	if channel != allChannel:
		channel.lastMessage += text
	allChannel.lastMessage += text


func _apply_channel_text(channel : ConsoleChannel):
	if channelChanged:
		channelChanged = false
		consoleText.bbcode_text = channel.text
	else:
		if channel != allChannel:
			channel.text += channel.lastMessage
		allChannel.text += channel.lastMessage
		
		if selectedChannel == channel or selectedChannel == allChannel:
			consoleText.bbcode_text += channel.lastMessage
	channel.lastMessage = ""


func _apply_selected_channel_text():
	_apply_channel_text(selectedChannel)


func get_last_message() -> String:
	return enteredMessages.back()


func get_command(command : String) -> ConsoleCommand:
	var cmdName = command.split(" ", false)[0] 
	
	for com in commands:
		if com.get_invoke_name() == cmdName:
			return com
	return null # if not found


func extract_arguments(msg : String) -> Array:
	var args = msg.split(" ", false)
	args.remove(0)
	return args


func print_args(commandIndex : int):
	var selectedCommand : ConsoleCommand = commands[commandIndex]
	
	if selectedCommand.get_expected_args().size() == 0:
		write("0")
	
	elif CommandRef.VARIANT in selectedCommand.get_expected_args():
		write("unspecified amount")
	elif CommandRef.ALIAS in selectedCommand.get_expected_args():
		write("aliased amount")
	
	elif selectedCommand.get_expected_args().size() > 0:
		for argNum in selectedCommand.get_expected_args():
			write(str(argNum))
			if selectedCommand.get_expected_args().size() > 2 and \
					argNum == selectedCommand.get_expected_args()[selectedCommand.get_expected_args().size() - 2]:
				write(" or %s" % selectedCommand.get_expected_args().back())
				break
			else:
				if selectedCommand.get_expected_args().size() > 1:
					write(", ")


func copy_command(command : ConsoleCommand) -> ConsoleCommand:
	var newCommand = ConsoleCommand.new(\
			command.get_invoke_name(), command.get_ref(),\
			command.get_description(), command.get_default_args(), command.get_call_rights())
	return newCommand


func get_channel(channelName : String) -> ConsoleChannel:
	for channel in channelTexts:
		if channel.channelName == channelName:
			return channel
	return null


func write_line_channel(channelName : String, text : String, flags = 0):
	write_channel(channelName, text + "\n", flags)


func write_channel(channelName : String, text : String, flags = 0):
	var channel = get_channel(channelName)
	if !channel:
		var _c = add_channel(channelName)
	write(text, flags, channel)


func _write_to_channel(channel : ConsoleChannel, text : String):
	channel.lastMessage += text


func get_channel_text(channelName : String):
	for channel in channelTexts:
		if channel.channelName == channelName:
			return channel.text
	return ""


func execute_command(message : String):
	var currentCommand = message
	currentCommand = currentCommand.trim_prefix(commandSign) # remove command sign
	# return the command and the whole message
	var cmd : ConsoleCommand = get_command(currentCommand)
	if cmd == null:
		write_line("")
		write(COMMAND_NOT_FOUND_MSG)
		return
		
	var found = false
	for i in range(commands.size()):
		if commands[i].get_invoke_name() == cmd.get_invoke_name(): # found command
			found = true
			if not cmd.are_rights_sufficient(userRight):
				write("Not sufficient rights as %s." % ConsoleUserRight.TypeDict[userRight])
				break
			
			var args = extract_arguments(currentCommand)
				
			if not args.size() in cmd.get_ref().get_expected_arguments() and \
					 not CommandRef.VARIANT in cmd.get_ref().get_expected_arguments() and \
					 not CommandRef.ALIAS in cmd.get_ref().get_expected_arguments():
				write_line("")
				write("expected: ")
				print_args(i)
				write(" arguments!")
				break
			
			found = true
			cmd.apply(args)
			emit_signal("on_command_sent", cmd, currentCommand)
			break
	
	if not found:
		write_line("")
		write(COMMAND_NOT_FOUND_MSG)


func send_line_input():
	var flags = ConsoleFlags.Type.USER_PREFIX * int(sendUserName) | \
				ConsoleFlags.Type.TRACK_MESSAGE 
	
	if showTimeStamp:
		flags |= ConsoleFlags.Type.SHOW_TIME
	
	write_line(consoleLine.text, flags)
	consoleLine.text = ""


func _input(event):
	if event is InputEventKey:
		if event.is_pressed() and not event.is_echo():
			if event.scancode == toggleConsoleKey:
				toggle_console()
			if event.scancode == KEY_ENTER:
				if not consoleLine.text.empty():
					_on_Line_text_entered($Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/Input/Line.text)
			elif event.scancode == KEY_ESCAPE:
				consoleLine.text = ""
			elif event.scancode == KEY_LEFT and Input.is_key_pressed(KEY_CONTROL) and \
					consoleLine.get_cursor_position() == 0:
				consoleLine.set_cursor_position(consoleLine.text.length())
		if event.is_action_pressed(previousMessageKey):
			if enteredMessages.empty():
				return
			messageIndex -= 1
			if messageIndex < 0:
				messageIndex = enteredMessages.size() - 1
			elif messageIndex > enteredMessages.size() - 1:
				messageIndex = 0
			consoleLine.text = enteredMessages[messageIndex]
			consoleLine.grab_focus()
			consoleLine.set_cursor_position(consoleLine.text.length())
		
		elif event.is_action_pressed(nextMessageKey):
			if enteredMessages.empty():
				return
			messageIndex += 1
			if messageIndex < 0:
				messageIndex = 0
			elif messageIndex > enteredMessages.size() - 1:
				messageIndex = 0
			consoleLine.text = enteredMessages[messageIndex]
	
		if event.is_action_pressed(autoCompleteKey):
			if consoleLine.text.length() > 1:
				var closests = get_closest_commands(consoleLine.text)
				if  closests != null:
					if closests.size() == 1:
						consoleLine.text = commandSign + closests[0]
						consoleLine.set_cursor_position(consoleLine.text.length())
					elif closests.size() > 1:
						var tempLine = consoleLine.text
						write_line("possible commands:")
						consoleText.newline()
						
						for c in closests:
							consoleText.newline()
							write_line(commandSign + c, ConsoleFlags.Type.CLICKABLE | ConsoleFlags.Type.NO_COMMANDS)

						#send_message_without_event("Press [Up] or [Down] to cycle through available commands.", false)
						consoleLine.text = tempLine
						consoleLine.set_cursor_position(consoleLine.text.length())


func add_command(command : ConsoleCommand):
	commands.append(command)


func get_closest_commands(command : String) -> Array:
	if command.empty() or command[0] != commandSign:
		return []
	
	var results = []
	var cmdName : String = command.split(" ", false)[0]
	cmdName = cmdName.trim_prefix(commandSign)
		
	for com in commands:
		if com.get_invoke_name().length() < cmdName.length():
			continue
		var addToResults = true
		for i in range(cmdName.length()):
			if not cmdName[i].to_lower() == com.get_invoke_name()[i].to_lower():
				addToResults = false
				break
				
		if addToResults:
			results.append(com.get_invoke_name())
				
	return results



### LOGGING


func add_to_log(msg : String):
	if not logFileCreated:
		return
	completeText += get_time_stamp() + logType + msg + "\n"
	logType = ""


func create_log_file():
	var dir := Directory.new()
	var resPath = str(get_script().resource_path)
	logFilePath = resPath.substr(0, resPath.find_last("/") + 1) + "logs"
	if dir.make_dir(logFilePath) != OK:
		printerr("Couldn't create folder for console logs!")
	
	logFilePath += "/"
	
	logFile = File.new()
	if logFile.open(logFilePath + logFileName, logFile.WRITE_READ) != OK:
		write_line("[color=red]Couldn't create File![/color]")
	logFileCreated = true
	logFile.close()


func append_log_to_file(fileName : String = ""):
	if not logEnabled:
		return
	
	if not fileName == "":
		logFileName = fileName
	
	if logFile.open(logFilePath + logFileName, logFile.READ_WRITE) != OK:
		write_line("[color=red]Couldn't open log file![/color]")
	logFile.seek_end()
	logFile.store_string(completeText)
	logFile.close()
	completeText = ""


func enable_log():
	logEnabled = true


func disable_log():
	logEnabled = false



### EXPORT METHODS

func update_console_text_preview():
	if !showPreview:
		return
	if _check_for_tool("Control/VBoxContainer/ConsoleContainer/ConsoleText/RichTextLabel"):
		
		var nameText := ""
		if showTimeStamp:
			nameText += get_time_stamp(false)
		if sendUserName:
			nameText = str(userMsgSign, " ", color_to_bb_code(userName, userNameColor), ":")
		else:
			nameText = ""
		$Control/VBoxContainer/ConsoleContainer/ConsoleText/RichTextLabel.bbcode_text = \
				color_to_bb_code(previewConsoleText % [ \
					nameText, \
					str(" ", commandSign)], consoleTextColor)


func update_console_line_preview():
	if !showPreview:
		return
	if _check_for_tool("Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/Input/Line"):
		$Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/Input/Line.text = \
				previewLineMessage % str(userMsgSign, " ")


func update_user_name(newName : String):
	userName = newName
	update_console_text_preview()


func update_user_message_sign(newSign : String):
	userMsgSign = newSign
	update_console_line_preview()


func update_design(type): 
	design = type
	var designDict = ConsoleTheme.TypeDict[type]
	
	if designDict.dockingStation != Dockings.Type.CUSTOM:
		update_docking_station(designDict.dockingStation)
	
	update_rounded_title_bar(designDict.roundedTitleBar)
	
	update_show_send_button(designDict.showSendButton)
	update_show_line_background(designDict.showLineBackground)
	update_show_text_background(designDict.showTextBackground)
	update_show_title_bar(designDict.showTitleBar)
	
	update_send_button_text_color(designDict.sendButtonTextColor)
	update_background_color(designDict.backgroundColor)
	update_send_button_color(designDict.sendButtonColor)
	update_title_bar_color(designDict.titleBarColor)
	update_line_background_color(designDict.lineEditColor)
	update_text_color(designDict.textColor)
	
	slideInAnimation = designDict.animation
	hideScrollBar = designDict.hideScrollBar
	
	property_list_changed_notify()


func update_text_color(colorType):
	if typeof(colorType) == TYPE_COLOR: 
		consoleTextColor = colorType
	else:
		textColor = colorType
		consoleTextColor = ConsoleColor.TypeDict[colorType]
	update_console_line_preview()
	update_console_text_preview()


func update_show_scroll_bar(showScroll : bool):
	hideScrollBar = showScroll
	if _check_for_tool("Control/VBoxContainer/ConsoleContainer/ConsoleText/RichTextLabel"):
		$Control/VBoxContainer/ConsoleContainer/ConsoleText/RichTextLabel.scroll_active = !hideScrollBar


func update_user_command_sign(newSign : String):
	commandSign = newSign
	update_console_text_preview()


func update_send_user_name(send : bool):
	sendUserName = send
	update_console_text_preview()


func update_console_user_name_color(colorType):
	if typeof(colorType) == TYPE_COLOR:
		userNameColor = colorType
	else:
		consoleUserNameColor = colorType
		userNameColor = ConsoleColor.TypeDict[colorType]
		update_console_text_preview()


func update_show_console_background(showBg : bool):
	showConsoleBackground = showBg
	if _check_for_tool("Control/VBoxContainer/ConsoleContainer/ConsoleBackground"):
		$Control/VBoxContainer/ConsoleContainer/ConsoleBackground.visible = showBg


func update_show_title_bar(showBar : bool):
	showTitleBar = showBar
	if _check_for_tool("Control/VBoxContainer/TitleBar"):
		$Control/VBoxContainer/TitleBar.visible = showBar


func update_show_send_button(showButton : bool):
	showSendButton = showButton
	if _check_for_tool("Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ButtonField/SendText") and \
			_check_for_tool("Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ButtonField"):
		$Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ButtonField.visible = showSendButton
		$Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ButtonField/SendText.visible = showSendButton


func update_show_channel_button(show : bool):
	showChannelButton = show
	if _check_for_tool("Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ChannelSelector"):
		$Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ChannelSelector.visible = show


func update_send_button_color(color : Color):
	sendButtonColor = color
	if _check_for_tool("Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ButtonField/SendText"):
		var btn = $Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ButtonField/SendText
		btn.get("custom_styles/normal").bg_color = color


func update_send_button_text_color(color : Color):
	sendButtonTextColor = color
	if _check_for_tool("Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ButtonField/SendText"):
		var btn = $Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ButtonField/SendText
		btn.set("custom_colors/font_color", color)


func update_rounded_title_bar(rounded : bool):
	roundedTitleBarEdges = rounded
	if _check_for_tool("Control/VBoxContainer/TitleBar/TitleBarBackground"):
		var tileBackground = $Control/VBoxContainer/TitleBar/TitleBarBackground
		
		if tileBackground.get("custom_styles/panel") == null:
			return
		
		var panel = tileBackground.get("custom_styles/panel")
		
		if rounded:
			panel.set("corner_radius_top_left", 7)
			panel.set("corner_radius_top_right", 7)
#			newStyle.set("corner_radius_top_left", 7)
#			newStyle.set("corner_radius_top_right", 7)
		if not rounded:
			panel.set("corner_radius_top_left", 0)
			panel.set("corner_radius_top_right", 0)


func update_title_bar_color(color : Color):
	titleBarBackgroundColor = color
	if _check_for_tool("Control/VBoxContainer/TitleBar/TitleBarBackground"):
		var tileBackground = $Control/VBoxContainer/TitleBar/TitleBarBackground
		
		if tileBackground.get("custom_styles/panel") == null:
			return
		
		tileBackground.get("custom_styles/panel").bg_color = titleBarBackgroundColor


func update_show_text_background(showBg : bool):
	showTextBackground = showBg
	if _check_for_tool("Control/VBoxContainer/ConsoleContainer/ConsoleText/TextBackground"):
		$Control/VBoxContainer/ConsoleContainer/ConsoleText/TextBackground.visible = showBg


func set_console_size(width : float, height : float):
	rect_size = Vector2(width, height)


func update_text_background_color(color : Color):
	textBackgroundColor = color
	if _check_for_tool("Control/VBoxContainer/ConsoleContainer/ConsoleText/TextBackground") and \
			_check_for_tool("Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ButtonField/ButtonBackground") and \
			_check_for_tool("Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ChannelSelector/Channels"):
		$Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ButtonField/ButtonBackground.color = color
		$Control/VBoxContainer/ConsoleContainer/ConsoleText/TextBackground.color = color
		$Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/ChannelSelector/Channels.get("custom_styles/normal").bg_color = color


func update_background_color(color : Color):
	backgroundColor = color
	if _check_for_tool("Control/VBoxContainer/ConsoleContainer/ConsoleBackground"):
		$Control/VBoxContainer/ConsoleContainer/ConsoleBackground.color = color


func update_show_line_background(showBackground : bool):
	showLineBackground = showBackground
	if _check_for_tool("Control/VBoxContainer/ConsoleContainer/InputLine/LineBackground"):
		$Control/VBoxContainer/ConsoleContainer/InputLine/LineBackground.visible = showLineBackground


func update_line_background_color(color : Color):
	lineBackgroundColor = color
	if _check_for_tool("Control/VBoxContainer/ConsoleContainer/InputLine/LineBackground"):
		$Control/VBoxContainer/ConsoleContainer/InputLine/LineBackground.color = color


func update_docking_station(newDocking):
	dockingStation = newDocking
	if !_check_for_tool("."):
		return
	if !is_inside_tree():
		return
	
	var rectSize : Vector2 = get_viewport_rect().size
	
	match dockingStation:
		Dockings.Type.TOP:
			rect_position = Vector2(0.0, 0.0)
			rect_size.x = rectSize.x
			rect_size.y = defaultConsoleSize.y
			update_show_title_bar(false)
		Dockings.Type.BOTTOM:
			rect_position = Vector2(0.0, rectSize.y - defaultConsoleSize.y)
			rect_size.y = defaultConsoleSize.y
			rect_size.x = rectSize.x
			update_show_title_bar(false)
		Dockings.Type.LEFT:
			rect_position = Vector2(0.0, 0.0)
			rect_size.x = rectSize.x * 0.5
			rect_size.y = rectSize.y
			update_show_title_bar(false)
		Dockings.Type.RIGHT:
			rect_position = Vector2(rectSize.x * 0.5,  0.0)
			rect_size.x = rectSize.x * 0.5
			rect_size.y = rectSize.y
			update_show_title_bar(false)
		Dockings.Type.FULL_SCREEN:
			rect_position = Vector2(0.0, 0.0)
			rect_size = rectSize
			update_show_title_bar(false)
		Dockings.Type.CUSTOM:
			rect_size = defaultConsoleSize
		_:
			printerr("Docking station not found: ", dockingStation)
	
	property_list_changed_notify() # in order to update the "Script Variables" (Properties) 


func update_show_time_stamp(showStamp : bool):
	showTimeStamp = showStamp
	update_console_text_preview()


func update_log_time_interval(newInterval : float):
	logInterval = newInterval
	if _check_for_tool("LogTimer"):
		$LogTimer.wait_time = newInterval



### HELPER METHODS

func color_to_bb_code(between : String, color : Color) -> String:
	var result := ""
	result = "[color=#%s]" % color.to_html()
	result = str(result, between, "[/color]")
	return result 


func _check_for_tool(node) -> bool:
	if has_node(node) and get_node(node) != null:
		return true
	return false


func get_time_stamp(includeDate : bool = true, \
		dateDelim : String = "[/]", timeDelim : String = "[:]") -> String:
	var stamp := ""
	
	if includeDate:
		var dateDict = OS.get_datetime()
		var day = dateDict.day
		var month = dateDict.month
		var year = dateDict.year
		var dateTime = dateDelim[0] + str(day) + dateDelim[1] + str(month) + dateDelim[1] + str(year) + dateDelim[2]
		
		stamp = str(stamp, dateTime, " ")
	
	var timeDict = OS.get_time()
	var hour = timeDict.hour
	var minute = timeDict.minute
	var seconds = timeDict.second
	var time = timeDelim[0] + str(hour) + timeDelim[1] + str(minute) + timeDelim[1] + str(seconds) + timeDelim[2]
	
	stamp = str(stamp, time, " ")
	
	return stamp



### Signals


func _on_Line_text_entered(text):
	if !text.empty() and not (text.length() == 1 and text[0] == commandSign):
		send_line_input()


func _on_Console_resized():
	match dockingStation:
		Dockings.Type.CUSTOM:
			defaultConsoleSize = rect_size
		Dockings.Type.TOP:
			defaultConsoleSize.y = rect_size.y
		Dockings.Type.BOTTOM:
			defaultConsoleSize.y = rect_size.y
		_:
			pass
	
	if _check_for_tool("Control/VBoxContainer/ConsoleContainer/InputLine") and \
			_check_for_tool("Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/Input/Line"):
		var lineContainer = $Control/VBoxContainer/ConsoleContainer/InputLine
		var line = $Control/VBoxContainer/ConsoleContainer/InputLine/HBoxContainer/Input/Line
		var cText = $Control/VBoxContainer/ConsoleContainer/ConsoleText
		
		cText.rect_size.y = cText.get_parent().rect_size.y - line.rect_size.y
		
		lineContainer.rect_position.y = \
				lineContainer.get_parent().rect_size.y - line.rect_size.y
		lineContainer.rect_size.y = line.rect_size.y


func _on_Line_text_changed(new_text : String):
	if new_text.length() > 0 and new_text[0] == commandSign:
		consoleLine.focus_next = consoleLine.get_path()
	else:
		consoleLine.focus_next = NodePath("")
		
	new_text = new_text.replace("%c" % toggleConsoleKey, "")
	var caretPos = consoleLine.caret_position
	consoleLine.set_text(new_text)
	consoleLine.caret_position = caretPos


func _on_RichTextLabel_meta_clicked(meta):
	consoleLine.text = meta.substr(0, meta.length())
	consoleLine.set_cursor_position(consoleLine.get_text().length())
	consoleLine.grab_focus()


func _on_TitleBar_gui_input(event):
	if allowWindowDrag:
		if event is InputEventMouseMotion:
			if dragging:
				rect_global_position = get_global_mouse_position() - startWindowDragPos
		if event is InputEventMouseButton and event.button_index == BUTTON_LEFT: 
			if event.pressed and not event.is_echo():
				startWindowDragPos = get_global_mouse_position() - rect_global_position
				dragging = true
			elif not event.pressed and not event.is_echo():
				dragging = false
				rect_global_position = get_global_mouse_position() - startWindowDragPos


func _on_LogTimer_timeout():
	if not logEnabled:
		return
	if logFile.open(logFilePath + logFileName, logFile.WRITE_READ) != OK:
		return
#	logFile.seek_end()
	logFile.store_string(completeText)
	logFile.close()


func _on_SendText_pressed():
	_on_Line_text_entered(consoleLine.text)
	consoleLine.grab_focus()


func _on_Channels_item_selected(id):
	for channel in channelTexts:
		if channel.channelName == channelsButton.get_item_text(id):
			if channel != selectedChannel:
				channelChanged = true
				selectedChannel = channel
				_apply_selected_channel_text()
			return


func _on_HideConsole_pressed():
	toggle_console()

