extends Node
class_name ExecPool

signal _task_finished

var _mutex: Mutex = null
var _finished: bool = false
var _tasks: Array = []
var _threads_available: Array = []
var _threads_in_work: Array   = []

const MAX_THREADS_QTY: int = 4
var _max_threads_qty: int = -1


func run( instance: Object, method: String, callback: String ) -> void:
	_add_task(instance, method, callback, null, true, false)


func submit_task(instance: Object, method: String, callback: String, parameter ) -> void:
	_add_task(instance, method, callback, parameter, false, false)


func submit_task_array_parameterized(instance: Object, method: String, callback: String, parameter: Array ) -> void:
	_add_task(instance, method, callback, parameter, callback, false, true)




func _init():
	_mutex    = Mutex.new()
	_finished = false
	_tasks    = []
	_threads_available = []
	_threads_in_work   = []
	
	if MAX_THREADS_QTY > 0:
		_max_threads_qty = MAX_THREADS_QTY
	else:
		var qty: int = OS.get_processor_count()
		if qty > 1:
			qty -= 1
		_max_threads_qty = qty


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _notification(what: int):
	if what == NOTIFICATION_PREDELETE:
		#_wait_for_workers_to_finish()
		pass


func queue_free() -> void:
	_mutex.lock()
	_finished = true
	_mutex.unlock()
	
	.queue_free()


func _add_task(instance: Object, method: String, callback: String, parameter = null, task_tag = null, no_argument = false, array_argument = false) -> void:
	if _finished:
		return
	
	_mutex.lock()
	
	_tasks.push_front( Task.new(instance, method, callback, parameter, no_argument, array_argument) )
	var t: Thread = _get_worker_thread()

	_mutex.unlock()

	if t != null:
		t.start( self, "_process_tasks" )



func _process_tasks():
	
	while true:
		_mutex.lock()
		var t: Task
		if _tasks.empty():
			t = null
			var ind: int = _threads_in_work.find( self )
			if ind >= 0:
				_threads_in_work.pop_at( ind )
				_threads_available.push_back( self )
		else:
			t = _tasks.back()
			_tasks.pop_back()
		
		_mutex.unlock()
		
		if t == null:
			return
		
		t.execute_task()
		
		_mutex.lock()
		var finished: bool = _finished
		_mutex.unlock()
		
		if finished:
			return
		
		emit_signal( "_task_finished", t )



func _get_worker_thread():
	var t: Thread
	
	if _threads_available.empty():
		var qty: int = _threads_in_work.size()
		if qty >= _max_threads_qty:
			t = null
		
		else:
			t = Thread.new()
			_threads_in_work.push_back( t )
	
	else:
		t = _threads_available.back()
		_threads_available.pop_back()
		_threads_in_work.push_back( t )
	
	return t


func _wait_for_workers_to_finish():
	pass


func _on_task_finished( task: Task ):
	pass


class Task:
	var object: Object
	var method_name: String
	var target_argument
	var callback_name: String
	var result
	var __no_argument: bool
	var __array_argument: bool

	func _init(instance: Object, method: String, callback_method: String, parameter, no_argument: bool, array_argument: bool):
		object          = instance
		method_name     = method
		callback_name   = callback_method
		target_argument = parameter
		result          = null
		__no_argument = no_argument
		__array_argument = array_argument


	func execute_task():
		if __no_argument:
			result = object.call(method_name)
		elif __array_argument:
			result = object.callv(method_name, target_argument)
		else:
			result = object.call(method_name, target_argument)

