--    __  ____  __________  ___       ____  ___    _____   ______  ____ _       __
--   / / / / / /_  __/ __ \/   |     / __ \/   |  /  _/ | / / __ )/ __ \ |     / /
--  / / / / /   / / / /_/ / /| |    / /_/ / /| |  / //  |/ / __  / / / / | /| / / 
-- / /_/ / /___/ / / _, _/ ___ |   / _, _/ ___ |_/ // /|  / /_/ / /_/ /| |/ |/ /  
-- \____/_____/_/ /_/ |_/_/  |_|  /_/ |_/_/  |_/___/_/ |_/_____/\____/ |__/|__/   

--Starting time. No need to change.
local Ticks = 0

--Time between ticks
local SleepDuration = 50

--Number of ticks until the board makes a full hue cycle
local TicksPerCycle = 40

--Number of full RGB fades that exist on the keyboard at any given time. Can have decimals.
local NumRGB = 2

--Direction of movement
local Reverse = false
--Vertical slope
--6/92 gives about 1 xkey per ykey, 0 is vertical, negative for other direction
local Slope = 0.1

--Vary slope over time for extra woop. 
--Make sure to set a slope, as this will be the maximum slope it varies to.
local SlinkySlopeEnabled = true
local SlinkySlopeTicks = 40

--Vary NumRGB over time for double extra woop. 
--Make sure to have a NumRGB greater than 1, as this will be the maximum it varies to.
local SlinkyFadeEnabled = false
local SlinkyFadeTicks = 60

function main()
        --Precalculate the slope we use this tick
        local ASlope = Slope
        if SlinkySlopeEnabled then
                ASlope = Slope * math.sin( 2*math.pi * math.fmod(Ticks, SlinkySlopeTicks) / SlinkySlopeTicks )
        end
        --Precalculate the number of RGBs we use this tick
        local ANumRGB = NumRGB
        if SlinkyFadeEnabled then
                ANumRGB = NumRGB * ( math.sin( 2*math.pi * math.fmod(Ticks, SlinkyFadeTicks) / SlinkyFadeTicks ) + 1 )/2
        end
        
        --Loop through all keys
        for x = 0, 92, 4 do
                for y = 0, 6, 1 do
                        --Calculate various kinds of hues for this key
                        --xkeyhue changes hue left to right
                        local xkeyhue = x / 92 * ANumRGB
                        --ykeyhue changes hue top to bottom, creating a slope
                        local ykeyhue = 0
                        if ASlope > 0 then
                                ykeyhue = (y / 6) * ASlope
                        elseif ASlope < 0 then
                                ykeyhue = 1-(y / 6) * -ASlope
                        end
                        
                        --timehue moves the entire thing over time
                        local timehue = math.fmod(Ticks, TicksPerCycle) / TicksPerCycle
                        if Reverse then
                                timehue = 1 - timehue
                        end
                        
                        --Our actual hue ranges 0<hue<3
                        local hue = math.fmod( xkeyhue + timehue + ykeyhue , 1) * 3
                        
                        --We have three ranges: red->green, green->blue and blue->red. Hence hue goes to three.
                        --Here we set the key's color!
                        if hue < 1 then --Range 1
                                local v = math.floor(hue*8)
                                SetLed( x, y, v, 7-v, 0 )
                        elseif hue >= 2 then --Range 3
                                local v = math.floor((hue - 2)*8)
                                SetLed( x, y, 0, v, 7-v )
                        else --Range 2
                                local v = math.floor((hue - 1)*8)
                                SetLed( x, y, 7-v, 0, v )
                        end                     
                end
        end
        
        --Increment by one and do the usual
        Ticks = Ticks + 1
        Update()
        Sleep(SleepDuration)
end

function keypress( Key )
	if Key == 32 then
		Reverse = not Reverse
	end
end