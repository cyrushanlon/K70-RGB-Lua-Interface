local SleepTimer = 50

local XPos = 0
local ColorR = 7
local ColorG = 0
local ColorB = 0
local GoRight = 1

function main()

	--Draw

	if XPos == 92 then 
		GoRight = -1 
	elseif XPos == 0 then
		GoRight = 1
	end

	XPos = XPos + (GoRight * 4) -- 4 is standard key width

    for y = 0, 6 do

    	SetLed(XPos, y, ColorR, ColorG, ColorB)

    end

    local Col = math.random(7)

	if Col == 1 then
		ColorR = 7
		ColorG = 0
		ColorB = 0
	elseif Col == 2 then
		ColorR = 7
		ColorG = 7
		ColorB = 0
	elseif Col == 3 then
		ColorR = 7
		ColorG = 7
		ColorB = 7
	elseif Col == 3 then
		ColorR = 0
		ColorG = 7
		ColorB = 7
	elseif Col == 5 then
		ColorR = 7
		ColorG = 0
		ColorB = 7
	elseif Col == 7 then
		ColorR = 0
		ColorG = 7
		ColorB = 0
	else 
		ColorR = 0
		ColorG = 0
		ColorB = 7
	end

	Update()
	Sleep(SleepTimer)
end
