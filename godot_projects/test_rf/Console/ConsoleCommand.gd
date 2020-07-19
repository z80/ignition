class_name ConsoleCommand

var invokeName : String
var commandRef 
var defaultArgs : Array
var description : String = ""
var necessaryCallRight


func _init(invName : String, cRef, \
		cdescription : String = "", \
		cdefaultArgs : Array = [], \
		cnecessaryCallRight = ConsoleUserRight.Type.USER):
	
	self.invokeName = invName
	self.commandRef = cRef
	self.defaultArgs = cdefaultArgs
	self.description = cdescription 
	self.necessaryCallRight = cnecessaryCallRight


func apply(args : Array):
	if args.empty():
		commandRef.apply(defaultArgs)
	else:
		commandRef.apply(args)


func set_invoke_name(name : String):
	invokeName = name


func set_command(cmdRef):
	commandRef = cmdRef


func set_default_args(args : Array):
	defaultArgs = args


func set_description(cdescription : String):
	description = cdescription


# set_call_rights( ConsoleRights.CallRights.DEV )
func set_call_rights(rights):
	necessaryCallRight = rights


func get_call_rights() -> int:
	return necessaryCallRight


func are_rights_sufficient(rights) -> bool:
	return rights >= necessaryCallRight 


func get_expected_args() -> Array:
	return commandRef.get_expected_arguments()


func get_ref():
	return commandRef


func get_invoke_name() -> String:
	return invokeName


func get_default_args() -> Array:
	return defaultArgs


func get_description() -> String:
	return description
