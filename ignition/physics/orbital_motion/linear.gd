

static func init( r: Vector3, v: Vector3, args: Dictionary ):
	args.r = r
	args.v = v
	args.e_x = v.normalized()
	args.e_y = args.e_x

static func process( dt: float, args: Dictionary ):
	args.r += args.v * dt
	return [ args.r, args.v ]


