
static func spline_init( ar_a: Array, ar_b: Array, T=1.0 ):
	var qty: int = len( ar_a )
	var spline_data = {
		t = 0.0, 
		T = T, 
		data = []
	}
	for i in range( qty ):
		# t = (x-x1)/(x2-x1)
		# a =  k1*(x2-x1)-(y2-y1)
		# b = -k2*(x2-x1)+(y2-y1)
		# y = (1-t)*y1 + t*y2 + t*(1-t)*( (1-t)*a + t*b )
		# For trajectory interpolation velocity and heading.
		# For velocity k1 and k2 are 0.
		# For heading also 0.
		# So we end up with only function values.
		var y1: float = ar_a[i]
		var y2: float = ar_b[i]
		var a: float = y1 - y2
		var b: float = -a
		var d = [y1, y2, a, b]
		spline_data.data.push_back( d )
	
	return spline_data


static func spline_update( data, dt ):
	data.t += dt
	#print( "spline t: ", data.t )
	if data.t >= data.T:
		var t = data.t - data.T
		data.t = data.T
		var at = spline_at( data )
		spline_recompute( data, at )
		data.t = t


static func spline_recompute( data, ar_b: Array ):
	var T: float = data.T
	var t: float = data.t / T
	var ar_a: Array = spline_at( data )
	var d = spline_init( ar_a, ar_b, T )
	data.t = 0.0
	data.T = T
	data.data = d.data



static func spline_at( data, integrate=false ):
	var T: float = data.T
	var t: float = data.t / T
	if t > 1.0:
		t = 1.0
	var ys = []
	var qty = len( data.data )
	for i in range( qty ):
		var d = data.data[i]
		var y1: float = d[0]
		var y2: float = d[1]
		var a:  float = d[2]
		var b:  float = d[3]
		var y: float = (1.0-t)*y1 + t*y2 + t*(1.0-t)*( (1.0-t)*a + t*b )
		ys.append( y )
		if integrate:
			var int_y: float = ((t*t*y2)/2+(t-t*t*0.5)*y1-(3.0*(b-a)*t*t*t*t+(8.0*a-4.0*b)*t*t*t-6.0*a*t*t)/12.0) * T
			ys.append( int_y )
	
	return ys



static func spline_array_at( data, tt: Array, integrate=false ):
	var t_save = data.t
	var qty = len( tt )
	
	var points = []
	for i in qty:
		data.t = t_save + tt[i]
		var d = spline_at( data, integrate )
		points.push_back( d )
	
	data.t = t_save
	
	return points



