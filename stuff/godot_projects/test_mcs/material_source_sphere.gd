
extends Reference



func material( at: Vector3 ):
	var r: float = at.length()
	if r < 3.0:
		return 1
	
	return 0


