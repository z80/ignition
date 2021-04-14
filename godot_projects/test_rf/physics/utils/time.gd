

const SECS_PER_YEAR: float = 31536000.0
const SECS_PER_MONTH: float = 2592000.0
const SECS_PER_DAY: float = 86400.0
const SECS_PER_HOUR: float = 3600.0
const SECS_PER_MINUTE: float = 60.0

static func seconds_to_str( t_total: float ):
	var t: float = t_total
	var years: int = int( floor( t / SECS_PER_YEAR ) )
	t -= years * SECS_PER_YEAR
	var months: int = int( floor( t / SECS_PER_MONTH ) )
	t -= months * SECS_PER_MONTH
	var days: int = int( floor( t / SECS_PER_DAY ) )
	t -= days * SECS_PER_DAY
	var hours: int = int( floor( t / SECS_PER_HOUR ) )
	t -= hours * SECS_PER_HOUR
	var minutes: int = int( floor( t / SECS_PER_MINUTE ) )
	var seconds: float = t - (minutes * SECS_PER_MINUTE)
	
	var stri: String = ""
	if years > 0:
		stri += "%dy," % [years]
	if months > 0:
		stri += "%dm," % [months]
	if days > 0:
		stri += "%dd," % [days]
	if hours > 0:
		stri += "%dh:" %[hours]
	if minutes > 0:
		stri += "%dm:" % [minutes]
	stri += "%2.1fs" % [seconds]
	
	return stri


