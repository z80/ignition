extends Node
class_name ConsoleDefaultCommands

var _firstHelp := true
var consoleRef   #: Console

onready var cdNode : Node = get_node("/root")



func set_default_commands(console):
	consoleRef = console
	
	var exitRef = CommandRef.new(self, "exit", 0)
	var exitCommand = ConsoleCommand.new('exit', exitRef, 'Closes the console.')
	console.add_command(exitCommand)
	
	var clearRef = CommandRef.new(self, "clear", 0)
	var clearCommand = ConsoleCommand.new('clear', clearRef, 'Clears the console.')
	console.add_command(clearCommand)
	
	var manRef = CommandRef.new(self, "man", 1)
	var manCommand = ConsoleCommand.new('man', manRef, 'shows command description.')
	console.add_command(manCommand)
	
	var helpRef = CommandRef.new(self, "help", 0)
	var helpCommand = ConsoleCommand.new('help', helpRef, 'shows all user defined commands.')
	console.add_command(helpCommand)
	
	var helpAllRef = CommandRef.new(self, "help_all", 0)
	var helpAllCommand = ConsoleCommand.new('helpAll', helpAllRef, 'shows all commands.')
	console.add_command(helpAllCommand)
	
	var setCommandSignRef = CommandRef.new(self, "set_command_sign", 1)
	var setCommandSignCommand = ConsoleCommand.new('setCommandSign', setCommandSignRef, 'Sets new command sign. (default: \'/\')')
	console.add_command(setCommandSignCommand)
	
	var toggleButtonRef = CommandRef.new(self, "toggle_button", 0)
	var toggleButtonCommand = ConsoleCommand.new('toggleButton', toggleButtonRef, 'Toggles visibility of \'send\' button.')
	console.add_command(toggleButtonCommand)
	
	var toggleEditLineRef = CommandRef.new(self, "toggle_edit_line", 0)
	var toggleEditLineCommand = ConsoleCommand.new('toggleShowEditLine', toggleEditLineRef, 'Toggles visibility of edit line.')
	console.add_command(toggleEditLineCommand)
	
	var setUserMessageSignRef = CommandRef.new(self, "set_user_msg_sign", 1)
	var setUserMessageSignCommand = ConsoleCommand.new('setUserMessageSign', setUserMessageSignRef, 'Sets new sign for user messages. (default: \'>\')')
	console.add_command(setUserMessageSignCommand)
	
	var toggleWindowDragRef = CommandRef.new(self, "toggle_window_drag", 0)
	var toggleWindowDragCommand = ConsoleCommand.new('toggleWindowDrag', toggleWindowDragRef, 'Toggles whether the console is draggable or not.')
	console.add_command(toggleWindowDragCommand)
	
	var setThemeRef = CommandRef.new(self, "set_theme", 1)
	var setThemeCommand = ConsoleCommand.new('setTheme', setThemeRef, 'Sets the theme (number).')
	console.add_command(setThemeCommand)
	
	var setDockRef = CommandRef.new(self, "set_dock", 1)
	var setDockCommand = ConsoleCommand.new('setDock', setDockRef, 'Sets the docking station.')
	console.add_command(setDockCommand)
	
	var setTextColorRef = CommandRef.new(self, "set_default_text_color", 1)
	var setTextColorCommand = ConsoleCommand.new('setDefaultTextColor', setTextColorRef, 'Sets the default text color (format: #rrggbb).')
	console.add_command(setTextColorCommand)

	var aliasRef = CommandRef.new(self, "alias", CommandRef.VARIANT)
	var aliasCommand = ConsoleCommand.new('alias', aliasRef, 'Sets an alias for a command\narg 1: newname\narg 2: command.', [], ConsoleUserRight.Type.DEV)
	console.add_command(aliasCommand)
	
	var toggleTitlebarRef = CommandRef.new(self, "toggle_titlebar", 0)
	var toggleTitlebarCommand = ConsoleCommand.new('toggleTitlebar', toggleTitlebarRef, 'Toggles the titlebar.')
	console.add_command(toggleTitlebarCommand)
	
	var setConsoleSizeRef = CommandRef.new(self, "set_console_size", 2)
	var setConsoleSizeCommand = ConsoleCommand.new('setConsoleSize', setConsoleSizeRef, 'Sets the console size.')
	console.add_command(setConsoleSizeCommand)
	
	var toggleTextBackgroundRef = CommandRef.new(self, "toggle_text_background", 0)
	var toggleTextBackgroundCommand = ConsoleCommand.new('toggleTextBackground', toggleTextBackgroundRef, 'Toggles the text background.')
	console.add_command(toggleTextBackgroundCommand)
	
	var toggleConsoleBackgroundRef = CommandRef.new(self, "toggle_console_background", 0)
	var toggleConsoleBackgroundCommand = ConsoleCommand.new('toggleConsoleBackground', toggleConsoleBackgroundRef, 'Toggles the console background.')
	console.add_command(toggleConsoleBackgroundCommand)
	
	var toggleTimeStampRef = CommandRef.new(self, "toggle_time_stamp", 0)
	var toggleTimeStampCommand = ConsoleCommand.new('toggleTimeStamp', toggleTimeStampRef, 'Toggles the console background.')
	console.add_command(toggleTimeStampCommand)
	
	var setUserColorRef = CommandRef.new(self, "set_user_color", 1)
	var setUserColorCommand = ConsoleCommand.new('setUserColor', setUserColorRef, 'Sets the color of the users name. (format: #rrggbb)')
	console.add_command(setUserColorCommand)
	
	var showDefaultCommandsRef = CommandRef.new(self, "show_default_commands", 0)
	var helpDefaultCommand = ConsoleCommand.new('helpDefault', showDefaultCommandsRef, 'Shows only the default commands.')
	var showDefaultCommandsCommand = ConsoleCommand.new('showDefaultCommands', showDefaultCommandsRef, 'Shows only the default commands.')
	console.add_command(helpDefaultCommand)
	console.add_command(showDefaultCommandsCommand)
	
	var showTreeCR = CommandRef.new(self, "show_tree", [0,1])
	var showTreeC = ConsoleCommand.new('ls', showTreeCR, 'Shows child nodes of given param or root node when no param given.', [], ConsoleUserRight.Type.DEV)
	console.add_command(showTreeC)
	
	var cdCR = CommandRef.new(self, "change_dir", 1)
	var cdC = ConsoleCommand.new('cd', cdCR, 'Changes directory to the one specified.', [], ConsoleUserRight.Type.DEV)
	console.add_command(cdC)

	var pwdCR = CommandRef.new(self, "print_working_dir", 0)
	var pwdC = ConsoleCommand.new('pwd', pwdCR, 'Shows current directory.', [], ConsoleUserRight.Type.DEV)
	console.add_command(pwdC)

	var addChildCR = CommandRef.new(self, "_add_child", [1,2])
	var addChildC = ConsoleCommand.new("addChild", addChildCR, \
			str('Adds a a scene (@param1) as child to the node (@param2)', \
			'\nIf no second param given, it\'ll be added to the current node.'), [], ConsoleUserRight.Type.DEV)
	consoleRef.add_command(addChildC)

	var setCR = CommandRef.new(self, "_set_attribute", [2,3])
	var setC = ConsoleCommand.new("set", setCR, "Sets variable/attribute (@param1) of node with value (@param2) (@param3) or current Node if no second param given. (see Node.set)", [], ConsoleUserRight.Type.DEV)
	consoleRef.add_command(setC)

	var getCR = CommandRef.new(self, "_get_attribute", [1,2])
	var getC = ConsoleCommand.new("get", getCR, "Gets variable/attribute (@param1) of node (@param2) or current Node if no second param given. (Node.get)", [], ConsoleUserRight.Type.DEV)
	consoleRef.add_command(getC)

	var hasCR = CommandRef.new(self, "_has_attribute", [1,2])
	var hasC = ConsoleCommand.new("has", hasCR, "Looks for variable/attribute (@param1) of node (@param2) or current Node if no second param given. (Node.get)", [], ConsoleUserRight.Type.DEV)
	consoleRef.add_command(hasC)

	var treeCR = CommandRef.new(self, "_show_tree", 0)
	var treeC = ConsoleCommand.new("tree", treeCR, "Shows whole tree of nodes (starting from current node/dir).", [], ConsoleUserRight.Type.DEV)
	consoleRef.add_command(treeC)





func _show_tree():
	var node = cdNode
	consoleRef.write_line("")
	consoleRef.write_line("[color=red].[/color]")
	var output = []
	_show_children(output, node)
	for i in range(output.size()):
		consoleRef.write_line(output[output.size() - 1 - i])


func _show_children(output : Array, node, level = 0):
	var i = 0
	for child in node.get_children():
		var col 
		if i == 0 and child.get_child_count() == 0:
			col = "|   ".repeat(level) + "`- - " +  "[color=%s]%s[/color]"
		else:
			col = "|   ".repeat(level) + "|- - " +  "[color=%s]%s[/color]"
		i += 1
		
		_show_children(output, child, level + 1)
		if child is Control:
			output.append(col % ["lime", child.name])
		elif child is Node2D:
			output.append(col % ["#4169E1", child.name])
		elif child is Spatial:
			output.append(col % ["#FE4D4D", child.name])
		else:
			output.append(col % ["silver", child.name])


func _has_attribute(params : Array):
	consoleRef.write_line("")
	if params.size() == 1:
		var val = cdNode.get(params[0]) != null
		if val:
			consoleRef.write_line("%s has %s." % [cdNode.name, params[0]])
		else:
			consoleRef.write_line("%s not found!" % params[0])
		
	elif params.size() == 2:
		if cdNode.has_node(params[0]):
			var val = cdNode.get_node(params[0]).has_meta(params[1])
			if val:
				consoleRef.write_line("%s has %s." % [cdNode.get_node(params[0]).name, val])
			else:
				consoleRef.write_line("%s not found!" % val)
		else:
			consoleRef.write_line("No child node named: %s" % params[0])


func _get_attribute(params : Array):
	consoleRef.write_line("")
	if params.size() == 1:
		var val = cdNode.get(params[0])
		consoleRef.write_line("%s" % val)
	elif params.size() == 2:
		if cdNode.has_node(params[0]):
			var val = cdNode.get_node(params[0]).get(params[1])
			consoleRef.write_line("%s" % val)
		else:
			consoleRef.write_line("No child node named: %s" % params[0])


func _set_attribute(params : Array):
	if params.size() == 2:
		cdNode.set(params[0], _determine_type(params[1]))
		consoleRef.write_line("Set %s to %s" % [params[0], params[1]])
	elif params.size() == 3:
		if cdNode.has_node(params[2]):
			cdNode.get_node(params[2]).set(params[0], _determine_type(params[1]))
			consoleRef.write_line("Set %s to %s" % [params[0], params[1]])
		else:
			consoleRef.write_line("No child node named: %s" % params[2])


func _determine_type(val : String):
	if val.is_valid_float():
		return val.to_float()
	elif val.is_valid_integer() or val.is_valid_hex_number():
		return val.to_int()
	else:
		return val


func _add_child(params : Array):
	if params.size() == 1:
		var child = load(params[0]).instance()
		if child != null:
			cdNode.add_child(load(params[0]).instance())
		else:
			consoleRef.write_line("Couldn't find %s" % params[1])
		
	elif params.size() == 2:
		var child = load(params[1]).instance()
		if child != null:
			if cdNode.has_node(params[0]):
				
				cdNode.get_node(params[0]).add_child(child)
			else:
				consoleRef.write_line("No child node named: %s" % params[0])
		else:
			consoleRef.write_line("Couldn't find %s" % params[1])


func print_working_dir():
	consoleRef.write_line("")
	consoleRef.write_line(str(cdNode.get_path()), ConsoleFlags.Type.NO_COMMANDS)


func change_dir(path : String):
	if path == "..":
		consoleRef.write_line("")
		if str(cdNode.get_path()).find_last("/") != 0:
			cdNode = get_node(str(cdNode.get_path()).substr(0, str(cdNode.get_path()).find_last("/")))
			consoleRef.write_line("Child nodes:")
			consoleRef.write_line("/ls", ConsoleFlags.Type.NO_DRAW)
		else:
			consoleRef.write_line("You are already at /root")
			consoleRef.write_line("Child nodes:")
			consoleRef.write_line("/ls", ConsoleFlags.Type.NO_DRAW)
	else:
		var node = get_node(cdNode.get_path())
		if node.has_node(path):
			consoleRef.write_line("")
			cdNode = node.get_node(str(str(cdNode.get_path()), "/", path))
			consoleRef.write_line("Child nodes:")
			consoleRef.write_line("/ls", ConsoleFlags.Type.NO_DRAW)
		else:
			consoleRef.write_line("")
			consoleRef.write_line("[color=red]No child node named %s[/color]" % [path])
			consoleRef.write_line("Child nodes:")
			consoleRef.write_line("/ls", ConsoleFlags.Type.NO_DRAW)


func show_tree(input : Array):
	if input.size() == 0:
		consoleRef.write_line("")
		for child in get_node(cdNode.get_path()).get_children():
			consoleRef.write_line("%s" % child.name, ConsoleFlags.Type.NO_COMMANDS | ConsoleFlags.Type.INDENT)
	elif input.size() == 1:
		if get_node(cdNode.get_path()).has_node(input[0]):
			consoleRef.write_line("")
			for child in get_node(cdNode.get_path()).get_node(input[0]).get_children():
				consoleRef.write_line("%s" % child.name, ConsoleFlags.Type.NO_COMMANDS | ConsoleFlags.Type.INDENT)
		else:
			consoleRef.write_line("")
			consoleRef.write_line("Couldn't find node %s" % input[0])
	else:
		return


func set_user_color(color : String):
	if color.is_valid_html_color():
		consoleRef.update_console_user_name_color(Color(color))


func show_default_commands():
	consoleRef.write_line("")
	for i in range(consoleRef.basicCommandsAmount):
		consoleRef.write("%s%s" % [consoleRef.commandSign, consoleRef.commands[i].get_invoke_name()], \
				ConsoleFlags.Type.NO_COMMANDS | ConsoleFlags.Type.CLICKABLE)
		consoleRef.write(": %s" % consoleRef.commands[i].get_description())
		consoleRef.write(" (args: ")
		consoleRef.print_args(i)
		consoleRef.write(")")
		consoleRef.write_line("")


func toggle_text_background():
	consoleRef.update_show_text_background(!consoleRef.showTextBackground)


func toggle_console_background():
	consoleRef.update_show_console_background(!consoleRef.showConsoleBackground)


func toggle_time_stamp():
	consoleRef.update_show_time_stamp(!consoleRef.showTimeStamp)


func set_console_size(width, height):
	consoleRef.rect_size = Vector2(width, height)


func toggle_titlebar():
	consoleRef.update_show_title_bar(!consoleRef.showTitleBar)


func alias(input : Array):
	if input.size() < 2:
		consoleRef.write_line("")
		consoleRef.write("not enough arguments! (received: %s)" % str(input.size()))
		return
	
	var cmd = consoleRef.get_command(input[1])
	if cmd == null:
		consoleRef.write_line("%s" % input[1])
		consoleRef.write(consoleRef.COMMAND_NOT_FOUND_MSG)
		return
		
	var command = consoleRef.copy_command(cmd)
	command.set_invoke_name(input[0])
	if input.size() > 2:
		var _args : Array
		for ti in range(input.size() - 2):
			var i = ti + 2
			_args.append(input[i])
		command.set_default_args(_args)
		command.get_ref().get_expected_arguments().append(CommandRef.ALIAS)
		
	consoleRef.add_command(command)
	

func set_default_text_color(color : String):
	if color.is_valid_html_color():
		consoleRef.update_text_color(Color(color))


func set_dock(docking):
	if Dockings.TypeDict.has(docking):
		consoleRef.update_docking_station(Dockings.TypeDict[docking])
	
	
func set_theme(snum):
	var num = int(snum)
	if ConsoleTheme.Type.SIZE > num and num >= 0:
		consoleRef.update_design(num)
	else:
		consoleRef.write_line("No theme with that number.")


func help_all():
	consoleRef.write_line("")
	for i in range(consoleRef.commands.size()):
		consoleRef.write("%s%s" % [consoleRef.commandSign, consoleRef.commands[i].get_invoke_name()], \
				ConsoleFlags.Type.NO_COMMANDS | ConsoleFlags.Type.CLICKABLE)
		consoleRef.write(": %s" % consoleRef.commands[i].get_description())
		consoleRef.write(" (args: ")
		consoleRef.print_args(i)
		consoleRef.write(")")
		consoleRef.write_line("")


func set_command_sign(newSign : String):
	consoleRef.commandSign = newSign


func toggle_button():
	consoleRef.update_show_send_button(!consoleRef.showSendButton)


func toggle_edit_line():
	consoleRef.update_show_line_background(!consoleRef.showLineBackground)


func set_user_msg_sign(userSign : String):
	consoleRef.update_user_message_sign(userSign)


func toggle_window_drag():
	consoleRef.allowWindowDrag = !consoleRef.allowWindowDrag


func clear():
	consoleRef.clear_selected_channel_text()


func exit():
	consoleRef.toggle_console()


func man(command : String):
	consoleRef.write_line("")
	for i in range(consoleRef.commands.size()):
		if consoleRef.commands[i].get_invoke_name() == command:
			consoleRef.write("%s%s" % [consoleRef.commandSign, consoleRef.commands[i].get_invoke_name()], \
					ConsoleFlags.Type.NO_COMMANDS | ConsoleFlags.Type.CLICKABLE)
			consoleRef.write(": %s" % consoleRef.commands[i].get_description())
			consoleRef.write(" (args: ")
			consoleRef.print_args(i)
			consoleRef.write(")")
			consoleRef.write_line("")
			return
	
	consoleRef.write_line("[color=red]Couldn't find command '%s'[/color]" % command)
		
	
func help():
	consoleRef.write_line("")
	if _firstHelp:
		_firstHelp = false
		consoleRef.write("'help' shows user added commands. Use 'helpAll' to show all commands")
	
	for ti in range(consoleRef.commands.size() - consoleRef.basicCommandsAmount):
		var i = ti + consoleRef.basicCommandsAmount
		consoleRef.write_line("")
		consoleRef.write("%s%s" % [consoleRef.commandSign, consoleRef.commands[i].get_invoke_name()], \
				ConsoleFlags.Type.NO_COMMANDS | ConsoleFlags.Type.CLICKABLE)
		consoleRef.write(": %s" % consoleRef.commands[i].get_description())
		consoleRef.write(" (args: ")
		consoleRef.print_args(i)
		consoleRef.write(")")
		
