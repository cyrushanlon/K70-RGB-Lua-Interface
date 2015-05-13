local x = 0
local dx = 4

function main()
	x = x + dx

	if x >= 92 then  --makes x move left to right
		dx = -dx
		x = 92
	elseif x <= 0 then
		dx = -dx
		x = 0
	end

	for i=0, 7, 1 do
		SetLed(x,i,0,7,0)
		SetLed(x - (dx*2), i, 7,7,7)
	end

	Update()
	Sleep(25)
end