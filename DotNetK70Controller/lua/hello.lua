
function main()
	Update()
	Sleep(25)
end

function keypress( key )
	SetLed(key, 7,0,0)
end

function keyrelease( key )
	SetLed(key, 0,0,0)
end