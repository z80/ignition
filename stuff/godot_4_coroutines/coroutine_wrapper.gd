
#simple wrapper that helps you in situations where you still need
#behavior similar to GDScriptFunctionState
class CoroutineWrapper:
    signal resumed
    signal stopped

    var completed = false

    func _init():
        stopped.connect(_on_stopped)
        run()

    func resume():
        resumed.emit()

    func run():
        pass

    func _on_stopped():
        for item in [resumed, stopped]:
            var sig: Signal = item
            for connection in sig.get_connections():
                var callable: Callable = connection.callable
                sig.disconnect(callable)
        completed = true

