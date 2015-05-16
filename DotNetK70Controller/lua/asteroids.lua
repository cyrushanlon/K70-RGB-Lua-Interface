--Time between ticks
local SleepDuration = 25
--SpawnDelay of new asteroids in ticks
local SpawnDelay = 200

--What range of keys to use
local leftcol = 0
local rightcol = 92
local toprow = 2
local botrow = 5

--Size of playing field
local width = rightcol-leftcol+1
local height = botrow-toprow+1

--Game variables
local Tick = 0
local Score = 0
local Lost = false
local ExplosionRadius = 0
local roids = {}

--Initial ship position
local ShipX = math.floor(width/4)
local ShipY = math.floor(height/2)

--Spawn new roids
function newRoid()
	local r = {}
	r.x = width+1
	r.y = math.random(height)
	table.insert( roids , r )
end	

--Resets the game
function newGame()
	Tick = 0
	Score = 0
	Lost = false
	ExplosionRadius = 0
	roids = {}
	ShipX = math.floor(width/4)
	ShipY = math.floor(height/2)
	newRoid()
end
newGame()

--Movement of the ship
function moveForward()
	if ShipX <= width-4 then
		ShipX = ShipX + 4
	end
end
function moveBackward()
	if ShipX >= 4 then
		ShipX = ShipX - 4
	end
end
function moveUp()
	if ShipY > 1 then
		ShipY = ShipY - 1
	end
end
function moveDown()	
	if ShipY < height then
		ShipY = ShipY + 1
	end
end

--Autopilot
local AutoPilot = true
function CheckCollision()
	for k, v in pairs( roids ) do
		--If we're on the same row
		if v.y == ShipY then
			--If the asteroid is not behind the ship, and closing in front
			if ShipX < v.x and (v.x - ShipX) < 10 then
				--If we are on top or bottom, we can only evade one way.
				if ShipY == 1 then
					moveDown()
				elseif ShipY == height then
					moveUp()
				else
					local directon = math.random( 2 )
					if directon == 1 then
						moveDown()
					else
						moveUp()
					end
				end				
			end
		end
	end
end 


--Keypress event
--Arrowkeys move it and turn off autopilot
--Control turns autopilot on
--Space starts new game when you died
function keypress( key )
    if key == 38 then
		AutoPilot = false
		moveUp()
	elseif key == 40 then
		AutoPilot = false
		moveDown()
	elseif key == 37 then
		AutoPilot = false
		moveBackward()
	elseif key == 39 then
		AutoPilot = false
		moveForward()
	--Control key to turn on autopilot
	elseif key == 163 then
		AutoPilot = true
	--Space to restart
	elseif Lost and key == 32 then
		newGame()
	end
end

function keyrelease(key)

end

function main()	

	if Lost then	
		
		ExplosionRadius = ExplosionRadius + 0.3
		
		--Render explosion
		for x = 0, 92, 4 do
			for y = 0, 6, 1 do
				local relx = (x - ShipX)/4
				local rely = y-ShipY
				local dist = math.sqrt( relx*relx + rely*rely )
				if ExplosionRadius > 30 then
					--Fade the red
					local hue = math.max( 0 , 7 - math.floor( (ExplosionRadius-30)/3 ) )
					SetLed( x, y, hue, 0, 0 )							
				elseif dist < ExplosionRadius then
					--Red
					SetLed( x, y, 7, 0, 0 )
				elseif dist < 2*ExplosionRadius then
					--Yellow to red
					local hue = math.floor( (dist - ExplosionRadius)/ExplosionRadius * 8 )
					SetLed( x, y, 7, hue, 0 )		
				else
					SetLed( x, y, 0, 0, 0 )
				end
			end
		end
		
		if ExplosionRadius > 50 then
			--render Score
			local ScoreStr = tostring(Score)
			local SlowTick = math.floor(Tick/20)
			local DisplayStage = math.fmod( SlowTick , string.len(ScoreStr)+1 )
			if DisplayStage == 0 then
				--Display green row
				for x = 5, 41, 4 do
					SetLed( x, 2, 0, 7, 0 )
				end
			else
				--Display the digit
				local char = tonumber(string.sub(ScoreStr , DisplayStage , DisplayStage))
				if char == 0 then
					char = 10
				end
				SetLed( 1+char*4, 2, 0, 7, 0 )
			end
		end
		
		--Auto-restart if we are on AutoPilot
		if ExplosionRadius > 250 and AutoPilot then
			newGame()
		end
	else

		--UPDATE GAMESTATE	
		Score = Score + 1
		
		for k, v in pairs( roids ) do
			--Move asteroid to the left
			v.x = v.x - 1		
			
			--Check for ship collision		
			if v.x == ShipX and v.y == ShipY then
				Lost = true
				print( "GAME OVER. Score: " .. tostring(Score) )
			end
			
			--Respawn roid if needed
			if v.x < 1 then
				v.x = width+1
				v.y = math.random(height)
				Score = Score + 5
			end
		end	
		
		--If we AutoPilot, do it
		if AutoPilot then
			CheckCollision()
		end
		
		--Spawn new roid every now and then
		if math.fmod( Tick , SpawnDelay ) == 0 then
			newRoid()
		end
		
		--RENDERING
		
		--Draw the background
		for x = 0, 92, 4 do
			for y = 0, 6, 1 do
				local shift = math.fmod( Tick/2+x , 92 )
				if shift < 30 then
					SetLed( x, y, 1, 0, 1 )
				elseif shift > 62 then	
					SetLed( x, y, 0, 0, 0 )
				else
					SetLed( x, y, 0, 1, 1 )
				end
			end
		end
		
		--Render the ship
		if AutoPilot then
			SetLed( leftcol+ShipX-1, toprow+ShipY-1, 7, 0, 3 )
		else
			SetLed( leftcol+ShipX-1, toprow+ShipY-1, 0, 3, 7 )
		end
		
		--Render the asteroids
		for k, v in pairs( roids ) do
			SetLed( leftcol+v.x-1, toprow+v.y-1, 7, 7, 7 )
		end
	end
	
	--STUFF
	Tick = Tick + 1
	
	Update()
	Sleep(SleepDuration)
end