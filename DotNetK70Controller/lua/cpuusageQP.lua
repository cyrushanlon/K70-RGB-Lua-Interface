local SleepDuration = 50
local Time = 500
local CPU = 0

function main()

	for x = 0, 92, 4 do -- Set Background colour
		for y = 1, 6, 1 do
			SetLed( x, y, 1, 7, 7 )
		end
	end

	if Time >= 100 then
		CPU = GetCPUUsage()
		CPU = math.floor(( CPU/10 ) + 0.5)
		Time = 0
	end
	
	for i = 1, CPU*4, 4 do -- 1 key is 4 wide
		SetLed( i+6, 3, 7, 0, 0 ) -- +6 to skip tab
	end


	Time = Time + SleepDuration

	Update()
	Sleep(SleepDuration)
end