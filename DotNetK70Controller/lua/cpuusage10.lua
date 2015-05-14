--Basic blue background with CPU usage
-- red 3, green 4 = 34%
-- yellow 7 = 77%

-- Variables out here are kept between calls
local SleepDuration = 50
local SampleCount = 40

local Time = 0
local Cur = 0

local CPUAvgVals = {}
local Ten = 0
local One = 0

function main()

	for x = 0, 92, 4 do -- Set Background colour
		for y = 0, 6, 1 do
			SetLed( x, y, 0, 0, 7 )
		end
	end

	if (Time % 100 == 0) then
		local CPU = GetCPUUsage()
		if (Cur == SampleCount) then -- stores 1 to 20 then loops back to the start
			Cur = 0
			CPUAvgVals[Cur] = CPU
		else
			CPUAvgVals[Cur] = CPU
			Cur = Cur + 1
		end
	end

	if (Time % 500 == 0) then -- every 500 ms change the displayed val

		local AvgCPU = 0
		for k,v in pairs(CPUAvgVals) do
			AvgCPU = AvgCPU + v
		end
		AvgCPU = AvgCPU / (#CPUAvgVals+ 1)

		Ten = math.floor(AvgCPU / 10)
		One = math.floor(AvgCPU - ( Ten * 10 ))
		if One < 0 then One = 0 end

		Time = 0

	end

	if (not (Ten == One)) then
		SetLed( Ten * 4 , 2, 7, 1, 1 )
		SetLed( One * 4 , 2, 1, 7, 1 )
	else
		SetLed( Ten * 4 , 2, 7, 7, 1 )
	end
		
	Time = Time + SleepDuration

	Update()
	Sleep(SleepDuration)
end