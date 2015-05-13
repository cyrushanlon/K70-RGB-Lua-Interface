# DotNetK70Controller
## Intro
The code will be messy while I work on it!

LED Controller for the Corsair K70 RGB that aims to allow users to create lua scripts that control the keyboard.

Make sure that the Corsair Utility Engine has the keyboard set to disable device lighting or the two sources will conflict and cause insane flickering.

## Lua Integration
There are example scripts in the lua folder.
Better documentation coming soon!

### C++ functions from within lua
Used to set a specified key to a specified colour.
```
x = int from 0 to 92 (Standard width key is 4 wide)
y = int from 0 to 6
r,g,b = int from 0 to 7

SetLed(x, y, r, g, b)
```
Sends the current LED matrix to the keyboard for display.
```
Update()
```
Sleeps for a specified number of milliseconds.
```
x = number 
Sleep(x) : 
```
Gets the current total CPU usage
```
GetCPUUsage()
```

## Requirements
The only external requirement to compile this application for Windows in VS2013 is lua 5.3.
Linux and OSX versions may be coming soon.

## Credit

Initial build taken from the great work found here: https://github.com/CalcProgrammer1/CorsairKeyboardSpectrograph
