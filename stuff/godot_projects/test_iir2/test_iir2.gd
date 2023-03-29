extends Node2D


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	test_iir2()


func test_iir2():
	var iir: Iir2sRef = Iir2sRef.new()
	var g: float = iir.gain
	print( "gain: ", g )
#	g *= 0.5
#	iir.gain = g

	
	iir.reset_to( [0.0] )
	var x: Array = [5.0]
	for i in range(500):
		var y: Array = iir.apply( x )
		if i % 10 == 0:
			print( str(i), ": ", y )

	print()

	x = [0.0]
	for i in range(500):
		var y: Array = iir.apply( x )
		if i % 10 == 0:
			print( str(i), ": ", y )
