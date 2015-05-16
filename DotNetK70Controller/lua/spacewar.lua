--Time between ticks
local SleepDuration = 25

--What range of keys to use
local leftcol = 3*4
local rightcol = 11*4
local toprow = 2
local botrow = 5

--Size of playing field
local width = rightcol-leftcol
local height = botrow-toprow

--Game variables
local Tick = 0
local ents = {}
local GameOver = false
local shipA = 1
local shipB = 2

--Spawn new entities
function newEnt( type , color , x , y  )
	local e = {}
	e.type = type
	e.color = color
	e.x = x
	e.y = y
	table.insert( ents , e )
end	

--Resets the game
function newGame()
	Tick = 0
	ents = {}
	GameOver = false
	
	--ShipA
	newEnt( "shipA" , {0,7,0} , 0 , math.floor(height/2)  )
	--ShipB
	newEnt( "shipB" , {0,0,7} , width , math.floor(height/2)+1  )
	
	--Since the array was empty, these are the table indices for the ships
	shipA = 1
	shipB = 2	
end
newGame()

--Movement of the ships
function moveUp( shipnr )
	if ents[shipnr].y > 1 then
		ents[shipnr].y = ents[shipnr].y - 1
	end
end
function moveDown( shipnr )	
	if ents[shipnr].y < height then
		ents[shipnr].y = ents[shipnr].y + 1
	end
end

function Shoot( shipnr )	
	if shipnr == 1 then
		newEnt( "BulletA" , {7,7,0} , ents[shipnr].x+4 , ents[shipnr].y  )
	elseif shipnr == 2 then
		newEnt( "BulletB" , {7,0,7} , ents[shipnr].x-4 , ents[shipnr].y  )
	end
end

--[[VK_LCONTROL
162

VK_LSHIFT
160

VK_TAB
9


VK_UP
38

VK_DOWN
40

VK_RCONTROL
163]]--

--Keypress event
function keypress( key )
	if GameOver then
		--Space to restart
		if key == 32 then
			newGame()
		end
	else
		--ShipA up
		if key == 9 then
			moveUp( shipA )
		--ShipA down
		elseif key == 160 then
			moveDown( shipA )
		--ShipA shoot
		elseif key == 162 then
			Shoot( shipA )
		--ShipB up
		elseif key == 38 then
			moveUp( shipB )
		--ShipB down
		elseif key == 40 then
			moveDown( shipB )
		--ShipB shoot
		elseif key == 163 then
			Shoot( shipB )
		end
	end
end

--Detect entities that go off the board
function WithinBounds( ent )
	if (ent.x < 0 ) or (ent.x > 92 ) then
		return false
	else
		return true
	end
end

function main()	

	--UPDATE GAMESTATE	
	local todelete = false
	for k, v in pairs( ents ) do
		--Bullets must move and check for collision
		if v.type == "BulletA" then
			v.x = v.x+4
			if v.x == ents[shipB].x and v.y == ents[shipB].y then
				print("ShipB got hit!")
			end
		elseif v.type == "BulletB" then
			v.x = v.x-4
			if v.x == ents[shipA].x and v.y == ents[shipA].y then
				print("ShipA got hit!")
			end
		end
		
		--Bullets mustbe deleted if they are like totally off the board.
		--Can't delete things inside the loop so it only deletes one per tick which should be fine #ducttape.
		if string.sub( v.type , 1 , 6 ) == "Bullet" then
			if not WithinBounds(v) then
				todelete = k
			end
		end
	end
	if todelete then
		table.remove( ents , todelete)
	end
	
	--RENDERING

	--Render the entities
	for k, v in pairs( ents ) do
		if WithinBounds(v) then
			SetLed( leftcol+v.x-1, toprow+v.y-1, v.color[1] , v.color[2] , v.color[3] )
		end
	end
	
	--Stuff
	Tick = Tick + 1	
	Update()
	Sleep(SleepDuration)
end