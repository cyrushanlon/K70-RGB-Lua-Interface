local SleepDuration = 50

function main()
    for x = 0, 92, 4 do
        for y = 0, 6, 1 do

        	local Col = math.random(8)

        	if Col == 1 then
        		SetLed( x, y, 7, 0, 0 )
        	elseif Col == 2 then
        		SetLed( x, y, 7, 7, 0 )
        	elseif Col == 3 then
        		SetLed( x, y, 7, 7, 7 )
    		elseif Col == 3 then
    			SetLed( x, y, 0, 7, 7 )
			elseif Col == 5 then
				SetLed( x, y, 0, 0, 7 )
			elseif Col == 6 then
				SetLed( x, y, 0, 0, 7 )
			elseif Col == 7 then
				SetLed( x, y, 0, 7, 0 )
			else 
				SetLed( x, y, 7, 0, 7 )
			end
		end

    end
    Update()
    Sleep( SleepDuration )
end