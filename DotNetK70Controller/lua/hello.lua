
function main()
	Update()
	Sleep(25)
end

function keypress( key )
	print(key .. " down")
	SetLed(key, 7,0,0)
end

function keyrelease( key )
	print(key .. " up")
	SetLed(key, 0,0,0)
end