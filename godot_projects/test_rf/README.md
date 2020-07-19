# LinuxLikeConsole ([v3.0](https://github.com/cobrapitz/LinuxLikeConsole/tree/v3.0))
### Linux Like Console written in gdscript for Godot (Game Engine)
###### For additional features or problems just write an issue or Discord cobrapitz#2872


![Console](https://github.com/cobrapitz/LinuxLikeConsole/blob/master/showcase/consoleFront.PNG)

###### toggle with: key above tab

***

## [Showcase](https://github.com/cobrapitz/LinuxLikeConsole/wiki/Showcase)

## [Wiki](https://github.com/cobrapitz/LinuxLikeConsole/wiki)

## [Changelog](https://github.com/cobrapitz/LinuxLikeConsole/wiki/Changelog)

***

## Features:
* [x] Auto completion / suggestion
* [x] Custom commands
* [x] **(NEW)** Custom Channels (Default only 'All' Channel)
* [x] Custom/built in themes (arch theme, ubuntu theme, windows, light, dark, text_only)
* [x] Built in commands like
  * [x] man, tree, ls, cd, help, alias, setDock, clear, ... 
* [x] Predefined and runtime forwarded parameters (runtime forwarding is prioritized)
* [x] Easy BBcode support (like: [b]this is bold[/b])
* [x] Logging
* [x] Dragable console
* [x] Slide in animation
* [x] User rights (to restrict the usage of developer only commands)
* [x] Additional Visual and Logging functions (warn, error, sucess )
***

### Fully customizeable

![functions](https://github.com/cobrapitz/LinuxLikeConsole/blob/master/showcase/console10.PNG)

***


### How to create custom commands

##### [Full Version here](https://github.com/cobrapitz/LinuxLikeConsole/wiki/Examples#1-how-to-add-custom-command-1)
```gdscript
#short version

onready var console = $Console

func _ready():
    var printThreeRef = CommandRef.new(self, "my_three_print", 3)
    var printThreeCommand = ConsoleCommand.new('printThree', printThreeRef , 'Custom print.')
    console.add_command(printThreeCommand )

# 3-arguments version (called with: /printVariant print this please)
func my_three_print(arg1, arg2, arg3):
    print("your args: %s %s %s" % [arg1, arg2, arg3]) 
```
