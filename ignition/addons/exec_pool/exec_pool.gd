
extends Node
class_name ExecPool

export(Resource) var settings = null setget _set_settings, _get_settings

var _mutex: Mutex = null
var _semaphore: Semaphore = null

var _finished: bool = false
var _tasks: Array = []
var _finished_tasks: Array = []
var _threads_in_work: Array   = []

var _max_threads_qty: int = -1



func push_back( instance: Object, method: String, callback: String ) -> void:
	_add_task( instance, method, callback, null, true, false )

func push_front( instance: Object, method: String, callback: String ) -> void:
	_add_task( instance, method, callback, null, true, false, true )


func push_back_with_arg( instance: Object, method: String, callback: String, parameter ) -> void:
	_add_task( instance, method, callback, parameter, false, false )

func push_front_with_arg( instance: Object, method: String, callback: String, parameter ) -> void:
	_add_task( instance, method, callback, parameter, false, false, true )


func push_back_with_args( instance: Object, method: String, callback: String, parameter: Array ) -> void:
	_add_task( instance, method, callback, parameter, false, true )

func push_front_with_args( instance: Object, method: String, callback: String, parameter: Array ) -> void:
	_add_task( instance, method, callback, parameter, false, true, true )


func _set_settings( s: Resource ):
	settings = s
	#property_list_changed_notify()
	#print( "settings: ", _settings )

func _get_settings():
	return settings


func _ready():
	_mutex     = Mutex.new()
	_semaphore = Semaphore.new()
	
	_finished = false
	_tasks    = []
	_finished_tasks = []
	_threads_in_work   = []
	
	if settings != null:
		_max_threads_qty = settings.max_threads_qty
	else:
		var qty: int = 1 #OS.get_processor_count()
		if qty > 1:
			qty -= 1
		_max_threads_qty = qty
	
	for i in range( _max_threads_qty ):
		_create_worker_thread()



func _process( _delta ):
	_mutex.lock()
	var task: Task
	var empty: bool = _finished_tasks.empty()
	
	if empty:
		task = null
	
	else:
		task = _finished_tasks.back()
		_finished_tasks.pop_back()
	_mutex.unlock()
	
	if task != null:
		#print( "finished tasks not empty, calling callback" )
		task.call_callback()
	
#	_print_stats()


func _notification(what: int):
	if what == NOTIFICATION_PREDELETE:
		_wait_for_workers_to_finish()



func queue_free() -> void:
	_mutex.lock()
	_finished = true
	_mutex.unlock()

	.queue_free()


func _add_task(instance: Object, method: String, callback: String, parameter = null, no_argument = false, array_argument = false, prioritized=false) -> void:
	if _finished:
		return
	
	_mutex.lock()
	
	var task: Task = Task.new(instance, method, callback, parameter, no_argument, array_argument)
	if prioritized:
		_tasks.push_back( task )
	else:
		_tasks.push_front( task )
	
	_semaphore.post()

	_mutex.unlock()
	
	#_print_stats()

#	if t != null:
#		var ret: int = t.start( self, "_process_tasks", t )
#		print( "task creation err code: ", ret )



func _process_tasks( thread: Thread ):
	
	while true:
		_mutex.lock()
		
		var task: Task
		var qty: int = _tasks.size()
		#print( "tasks in queue: ", qty )
		if _tasks.empty():
			task = null

		else:
			task = _tasks.back()
			_tasks.pop_back()
		
		_mutex.unlock()
		
		if task != null:
			#print( "executing task" )
			task.execute_task()
			#print( "executing task done" )
			
			_mutex.lock()
			if task != null:
				#print( "adding to finished tasks" )
				_finished_tasks.push_front( task )
			_mutex.unlock()
		
		
		_semaphore.wait()
		
		_mutex.lock()
		var finished: bool = _finished
		_mutex.unlock()
		
		if finished:
			return


func _create_worker_thread():
	var t: Thread
	
	t = Thread.new()
	_threads_in_work.push_back( t )
	var ret: int = t.start( self, "_process_tasks", t )
	print( "task creation err code: ", ret )
	
	return t


func _wait_for_workers_to_finish():
	_mutex.lock()
	_finished = true
	var threads: Array = _threads_in_work.duplicate()
	var qty: int = _threads_in_work.size()
	for i in range(qty):
		_semaphore.post()
	_mutex.unlock()
	
	for t in threads:
		var valid: bool = is_instance_valid( t )
		if valid:
			if t.is_active():
				t.wait_to_finish()



func _print_stats():
	_mutex.lock()
	var tasks_in_queue: int    = _tasks.size()
	var workers_running: int   = _threads_in_work.size()
	_mutex.unlock()
	
	print( "tasks in queue: ", tasks_in_queue, "; running qty: ", workers_running )



class Task:
	var object: Object
	var method_name: String
	var target_argument
	var callback_name: String
	var result
	var _no_argument: bool
	var _array_argument: bool
	
	
	func _init(instance: Object, method: String, callback_method: String, parameter, no_argument: bool, array_argument: bool):
		object          = instance
		method_name     = method
		callback_name   = callback_method
		target_argument = parameter
		result          = null
		_no_argument = no_argument
		_array_argument = array_argument


	func execute_task():
		if _no_argument:
			result = object.call(method_name)
		elif _array_argument:
			result = object.callv(method_name, target_argument)
		else:
			result = object.call(method_name, target_argument)
	
	
	func call_callback():
		var still_valid: bool = is_instance_valid( object )
		if still_valid:
			object.call( callback_name, result )





