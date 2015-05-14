function main()
    for x = 0, 92, 1 do
        for y = 0, 6, 1 do
            SetLed( x, y, math.random(7), math.random(7), math.random(7) )
        end
    end
    Update()
    Sleep( math.random(500) )
end