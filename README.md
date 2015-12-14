# K70 RGB Lua Interface
## Intro
Lua interface for controlling the K70 RGB.

Make sure that the Corsair Utility Engine is either off or has the lighting disabled or the two will conflict.

Place Lua scripts into a "lua" folder next to the exe
There are a few example scripts in the lua folder.

### C++ functions from Lua

Required for looping the lua script.
Without the Sleep it runs as fast as it can. Using sleep you can limit it.
```
function main()
  Update()
	Sleep(25)
end
```

Parameter is a key code sent from C++, the keycodes can be found here:

https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx

```
function keypress(key) 

end

function keyrelease(key) 

end
```

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
Lua 5.3 is built in, there should be no external requirements when building in VS2013.
Linux version may be coming soon.

## Credit

Initial build taken from the great work found here: https://github.com/CalcProgrammer1/CorsairKeyboardSpectrograph
