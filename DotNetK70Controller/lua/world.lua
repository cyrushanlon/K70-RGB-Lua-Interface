function main()

	for x = 0, 92, 4 do
		for y = 0, 7, 1 do
			SetLed(x,y,math.random(7),math.random(7),math.random(7))
		end
	end

	Update()
	Sleep(500)
end