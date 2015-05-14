local x = 0
local dx = 4

function main()


	SetLed(math.random(92),math.random(6),0,7,0)

	Update()
	Sleep(25)
end

function keypress( key )
	print( key )
end