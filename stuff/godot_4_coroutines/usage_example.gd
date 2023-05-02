
extends Node
var count_to_ten: CountToTen

class Abc extends CoroutineWrapper:
    func run():
        print("\tA")
        await resumed
        print("\tB")
        await resumed
        print("\tC")
        stopped.emit()

class CountToTen extends CoroutineWrapper:
    func run():
        var idx = 0
        while idx <= 10:
            print(idx)
            if idx == 5 or idx == 2:
                var abc = Abc.new()
                while abc.completed == false:
                    abc.resume()
                    await resumed
            await resumed
            idx += 1
        stopped.emit()

func _ready():
    count_to_ten = CountToTen.new()

func _process(delta):
    if count_to_ten == null:
        return

    if count_to_ten.completed:
        print("Stopped")
        count_to_ten = null
        return

    count_to_ten.resume()



