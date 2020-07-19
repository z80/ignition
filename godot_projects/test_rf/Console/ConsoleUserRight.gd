class_name ConsoleUserRight

enum Type {
	NONE = 0,
	USER = 1,
	TESTER = 2,
	MODERATOR = 4,
	ADMIN = 8,
	DEV = 16,
}


const TypeDict = {
	Type.NONE : "none",
	Type.USER : "user",
	Type.TESTER : "tester",
	Type.MODERATOR : "moderator",
	Type.ADMIN : "admin",
	Type.DEV : "dev",
}
