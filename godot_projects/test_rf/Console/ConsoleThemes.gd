class_name ConsoleTheme

enum Type {
	BLUE,
	LIGHT,
	DARK,
	GRAY,
	UBUNTU,
	ARCH_AQUA,
	ARCH_GREEN,
	WINDOWS,
	TEXT_ONLY,
}

const TypeDict = {
	Type.BLUE : {
		"dockingStation" : Dockings.Type.CUSTOM,
		
		"showSendButton" : false,
		"showLineBackground" : false,
		"showTitleBar" : true,
		"showTextBackground" : false,
		
		"titleBarColor" : Color(0, 0.18, 0.62, 0.95),
		"backgroundColor" : Color(0.09, 0.09, 0.16, 0.87),
		"lineEditColor" : Color(0.21, 0.21, 0.21, 0.82),
		"sendButtonColor" : Color(0.14, 0.14, 0.18, 0.34),
		"textBackgroundColor" : Color(0.0, 0.0, 0.0, 1.0),
		"textColor" : ConsoleColor.Type.WHITE,
		"sendButtonTextColor" : Color.white,
		
		"roundedTitleBar" : true,
		"animation" : "",
		"hideScrollBar" : false
	},
	Type.DARK : {
		"dockingStation" : Dockings.Type.CUSTOM,
		"showSendButton" : false,
		"showLineBackground" : false,
		"showTitleBar" : true,
		"showTextBackground" : false,
		"titleBarColor" : Color(0, 0, 0, 0.95),
		"roundedTitleBar" : true,
		"backgroundColor" : Color(0.06, 0.06, 0.08, 0.88),
		"lineEditColor" : Color(0.21, 0.21, 0.21, 0.82),
		"sendButtonColor" : Color(0.14, 0.14, 0.18, 0.34),
		"textBackgroundColor" : Color(0.0, 0.0, 0.0, 1.0),
		"textColor" : ConsoleColor.Type.WHITE,
		"sendButtonTextColor" : Color.white,
		
		"animation" : "",
		"hideScrollBar" : false
	},
	Type.LIGHT : {
		"dockingStation" : Dockings.Type.CUSTOM,
		"showSendButton" : false,
		"showLineBackground" : false,
		"showTitleBar" : true,
		"showTextBackground" : false,
		"titleBarColor" : Color(1.0, 1.0, 1.0, 0.95),
		"roundedTitleBar" : true,
		"backgroundColor" : Color(1.0, 1.0, 1.0, 0.5),
		"lineEditColor" : Color(0.87, 0.87, 0.87, 0.71),
		"sendButtonColor" : Color(0.14, 0.14, 0.18, 0.34),
		"textBackgroundColor" : Color(0.0, 0.0, 0.0, 1.0),
		"textColor" : ConsoleColor.Type.BLACK,
		"sendButtonTextColor" : Color.white,
		
		"animation" : "",
		"hideScrollBar" : false
	},
	Type.GRAY : {
		"dockingStation" : Dockings.Type.CUSTOM,
		"showSendButton" : false,
		"showLineBackground" : false,
		"showTitleBar" : true,
		"showTextBackground" : false,
		"titleBarColor" : Color(0.24, 0.24, 0.24, 0.95),
		"roundedTitleBar" : true,
		"backgroundColor" : Color(0.03, 0.03, 0.03, 0.5),
		"lineEditColor" : Color(0.21, 0.21, 0.21, 0.82),
		"sendButtonColor" : Color(0.14, 0.14, 0.18, 0.34),
		"textBackgroundColor" : Color(0.0, 0.0, 0.0, 1.0),
		"textColor" : ConsoleColor.Type.WHITE,
		"sendButtonTextColor" : Color.white,
		
		"animation" : "",
		"hideScrollBar" : false
	},
	Type.UBUNTU : {
		"dockingStation" : Dockings.Type.CUSTOM,
		"showSendButton" : false,
		"showLineBackground" : false,
		"showTitleBar" : true,
		"showTextBackground" : false,
		"titleBarColor" : Color(0.3, 0.3, 0.3, 0.95),
		"backgroundColor" : Color(0.26, 0.0, 0.27, 0.9),
		"lineEditColor" : Color(0.13, 0.0, 0.18, 0.77),
		"sendButtonColor" : Color(0.01, 0.01, 0.01, 0.34),
		"textBackgroundColor" : Color(0.0, 0.0, 0.0, 1.0),
		"textColor" : ConsoleColor.Type.WHITE,
		"sendButtonTextColor" : Color.white,
		
		"roundedTitleBar" : true,
		"animation" : "",
		"hideScrollBar" : false
	},
	Type.ARCH_AQUA : {
		"dockingStation" : Dockings.Type.CUSTOM,
		"showSendButton" : false,
		"showLineBackground" : false,
		"showTitleBar" : true,
		"showTextBackground" : false,
		"titleBarColor" : Color(0.35, 0.34, 0.34, 0.98),
		"roundedTitleBar" : true,
		"backgroundColor" : Color(0.0, 0.25, 0.38, 0.87),
		"lineEditColor" : Color(0.21, 0.35, 0.66, 0.82),
		"sendButtonColor" : Color(0.26, 0.27, 0.63, 0.34),
		"textBackgroundColor" : Color(0.0, 0.0, 0.0, 1.0),
		"textColor" : ConsoleColor.Type.AQUA,
		"sendButtonTextColor" : Color.white,
		
		"animation" : "",
		"hideScrollBar" : false
	},
	Type.ARCH_GREEN : {
		"dockingStation" : Dockings.Type.CUSTOM,
		"showSendButton" : false,
		"showLineBackground" : false,
		"showTitleBar" : true,
		"showTextBackground" : false,
		"titleBarColor" : Color(0.30, 0.27, 0.27, 1.0),
		"roundedTitleBar" : true,
		"backgroundColor" : Color(0.0, 0.0, 0.0, 0.98),
		"lineEditColor" : Color(0.24, 0.24, 0.24, 0.98),
		"sendButtonColor" : Color(0.3, 0.3, 0.32, 0.34),
		"textBackgroundColor" : Color(0.0, 0.0, 0.0, 1.0),
		"textColor" : ConsoleColor.Type.GREEN,
		"sendButtonTextColor" : Color.white,
		
		"animation" : "",
		"hideScrollBar" : false
	},
	Type.WINDOWS : {
		"dockingStation" : Dockings.Type.CUSTOM,
		"showSendButton" : false,
		"showLineBackground" : false,
		"showTitleBar" : true,
		"showTextBackground" : false,
		"titleBarColor" : Color(1.0, 1.0, 1.0, 1.0),
		"roundedTitleBar" : false,
		"backgroundColor" : Color(0.0, 0.0, 0.0, 1.0),
		"lineEditColor" : Color(0.11, 0.11,0.11, 0.82),
		"sendButtonColor" : Color(0.22, 0.22, 0.22, 0.34),
		"textBackgroundColor" : Color(0.0, 0.0, 0.0, 1.0),
		"textColor" : ConsoleColor.Type.WHITE,
		"sendButtonTextColor" : Color.white,
		
		"animation" : "",
		"hideScrollBar" : false
	},
	Type.TEXT_ONLY : {
		"dockingStation" : Dockings.Type.CUSTOM,
		"showSendButton" : false,
		"showLineBackground" : false,
		"showTitleBar" : false,
		"showTextBackground" : false,
		"titleBarColor" : Color(1.0, 1.0, 1.0, 0.0),
		"roundedTitleBar" : false,
		"backgroundColor" : Color(0.0, 0.0, 0.0, 0.0),
		"lineEditColor" : Color(0.11, 0.11,0.11, 0.0),
		"sendButtonColor" : Color(0.22, 0.22, 0.22, 0.0),
		"textBackgroundColor" : Color(0.0, 0.0, 0.0, 1.0),
		"textColor" : ConsoleColor.Type.WHITE,
		"sendButtonTextColor" : Color.white,
		
		"animation" : "",
		"hideScrollBar" : false
	} 
}
