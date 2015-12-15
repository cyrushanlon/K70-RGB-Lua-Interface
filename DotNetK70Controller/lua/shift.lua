--controls fps
local SleepDuration = 25
--controls length of each color
local TicksPerColor = 20
--controls length of fade between colors
local TicksPerFade = 5

--holds the colors that the keyboard switches through in order
--as follows: red orange yellow white, green, blue, purple
local Colors = { {7,0,0}, {7,4,0}, {7,7,0}, {7,7,7},{0,7,0}, {0,0,7}, {4,0,7}}

--current color in table
local CurrentColor = 1

--shouldnt be changed
--holds the next color index
local NextColor = 2

--holds the currently displayed color
local r = 7
local g = 7
local b = 7

--shouldnt be changed
--Holds how many ticks have passed since last reset
local CurrentTick = 0

--shouldnt be changed
--remeber how much to change rgb by to fade to next color
local FadeFirst = true
local rFade = 0
local bFade = 0
local gFade = 0

function main()

	CurrentTick = CurrentTick + 1

	if (CurrentTick > TicksPerColor) then -- change the color

		if (CurrentTick - TicksPerColor) > TicksPerFade then -- finished fading

			CurrentTick = 0
			FadeFirst = true

			CurrentColor = NextColor

			NextColor = NextColor + 1
			if NextColor > #Colors then 
				NextColor = 1
			end

			r = Colors[CurrentColor][1]
			g = Colors[CurrentColor][2]
			b = Colors[CurrentColor][3]

		else -- needs to fade

			if FadeFirst then
				rFade = (r - Colors[NextColor][1]) / TicksPerFade --(start - end) / num of ticks
				gFade = (g - Colors[NextColor][2]) / TicksPerFade
				bFade = (b - Colors[NextColor][3]) / TicksPerFade

				FadeFirst = false
			end

			r = r - rFade--next step in color change
			g = g - gFade--next step in color change
			b = b - bFade--next step in color change

		end
	end

	--fixes any odd rounding issues that may have shown up
	if r < 0 then r = 0 end
	if g < 0 then g = 0 end
	if b < 0 then b = 0 end

	-- go through all keys and sets them to current color
		for x = 0, 92, 4 do
			for y = 0, 6, 1 do
				SetLed(x, y, r, g, b)
			end
		end

	Update()
	Sleep(SleepDuration)
end

