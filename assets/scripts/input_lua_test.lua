while true do
    -- Check if C key was just pressed
    if Input.isKeyPressed(Input.KeyCode.C) then
        print("C key was pressed!")
    end
    
    -- Check for ESC key to exit
    if Input.isKeyPressed(Input.KeyCode.ESCAPE) then
        print("Exiting...")
        break
    end
    
    -- Small sleep to prevent maxing out CPU
    -- This function might need to be provided by your engine
    if Sleep then
        Sleep(0.016) -- ~60fps
    end
end