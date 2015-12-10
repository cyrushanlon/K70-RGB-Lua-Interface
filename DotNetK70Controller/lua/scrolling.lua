--
-- TEXT LOOKS BAD ON KEYBOARD!
--
local DisplayStrings = {}

DisplayStrings["Spacer"] = 
{
	'0',
	'0',
	'0',
	'0',
	'0',
}

DisplayStrings[" "] =
{
	'00000',
	'00000',
	'00000',
	'00000',
	'00000'
}
DisplayStrings["A"] = 
{ 
	'00100',
	'01010',
	'11111',
	'10001',
	'10001'
}
DisplayStrings["B"] =
{
	'11110',
	'10001',
	'11110',
	'10001',
	'11110'
}
DisplayStrings["C"] =
{
	'01111',
	'10000',
	'10000',
	'10000',
	'01111'
}	
DisplayStrings["D"] = 
{
	'11110',
	'10001',
	'10001',
	'10001',
	'11110'
}
DisplayStrings["M"] = 
{
	'10101',
	'11111',
	'10101',
	'10001',
	'10001'
}

local TargetString = "B"

local SleepDuration = 50

local FirstRun = true

local DisplayMatrix = {}

function AddSpacer()
	for k,v in pairs(DisplayStrings.Spacer) do
		DisplayMatrix[k] = DisplayMatrix[k] .. v
	end
end

function main() -- 0-92, 1-6

	if (FirstRun) then --generates display matrix

		DisplayMatrix = DisplayStrings.Spacer

		for i = 1, #TargetString do -- goes through TargetStrings Characters

			local c = DisplayStrings[TargetString:sub(i,i)] -- holds the character from DisplayStrings

			for k,v in pairs(c)  do -- adds the rows to the DisplayMatrix
				DisplayMatrix[k] = DisplayMatrix[k] .. v --adds the row to the matrix
			end

			--AddSpacer()

		end

		-- blanks the keyboard
		for y = 0, 6 do
			for x = 0, 92, 4 do

				SetLed(x,y,0,0,0)

			end
		end

		FirstRun = false

		Sleep(1000)

	end
	
	--prints the words
	for y, v in pairs(DisplayMatrix) do-- goes through the rows
		for x = 1, #v do-- goes through the columns

			if (v:sub(x,x) == '1') then
				SetLed(x*4, y+1, 0, 7, 0) --x y r g b
			else
				SetLed(x*4, y+1, 0, 0, 0) --x y r g b
			end

		end
	end

	--print("")
	--print("")
	--print("")
	--print("")
	--

	Update()
	Sleep(SleepDuration)
end