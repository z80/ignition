class_name CommandRef 


const VARIANT = -1
const ALIAS = -2

var funcRef := FuncRef.new()
var argsExpected # arguments for the function


func _init(_obj, _ref : String, _argsExpected = 0):
	funcRef.set_function(_ref)
	funcRef.set_instance(_obj)
	
	if typeof(_argsExpected) == TYPE_ARRAY:
		argsExpected = _argsExpected
	else:
		argsExpected = [_argsExpected]


func get_expected_arguments() -> Array:
	return self.argsExpected


func set_expected_arguments(args : Array):
	self.argsExpected = args


func get_max_args_expected() -> int:
	var maxNum = 0
	for num in argsExpected:
		if num > maxNum:
			maxNum = num
	return maxNum


func apply(args : Array):
	if VARIANT in argsExpected:
		funcRef.call_func(args)
		return
	elif ALIAS in argsExpected:
		if args.size() in get_expected_arguments():
			funcRef.call_funcv(args)
		return
	elif not args.size() in argsExpected:
		print("Arguments not matching")
		return
	
	if argsExpected.size() > 1:
		funcRef.call_func(args)
		return
	
	
	match args.size():
		0:
			funcRef.call_func()
		1:
			funcRef.call_func(args[0])
		2:
			funcRef.call_func(args[0], args[1])
		3:
			funcRef.call_func(args[0], args[1], args[2])
		4:
			funcRef.call_func(args[0], args[1], args[2], args[3])
		5:
			funcRef.call_func(args[0], args[1], args[2], args[3], args[4])
		6:
			funcRef.call_func(args[0], args[1], args[2], args[3], args[4], args[5])
		7:
			funcRef.call_func(args[0], args[1], args[2], args[3], args[4], args[5], args[6])
		8:
			funcRef.call_func(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7])
		9:
			funcRef.call_func(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9])
		10:
			funcRef.call_func(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10])
		_:
			printerr("If you want more than 10 args then pls just put more lines here, sorry! (here refers to ConsoleCommandRef.gd)")

