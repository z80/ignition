
extends "res://addons/gut/test.gd"


func test_check_dims():
	var stri = "fff -> ddd"
	var regex = RegEx.new()
	regex.compile("(?<src_name>\\S+)\\s*\\->\\s*(?<dest_name>\\S+)")

	var res = false
	var result = regex.search( stri )
	if result:
		print( result.get_string("src_name") )
		print( result.get_string("dest_name") )
		res = true
		
	assert_true( res )
	
	
	#regex.compile("(?<src_name>\\S+)\\s*\\->\\s*(?<dest_name>\\S+)")
