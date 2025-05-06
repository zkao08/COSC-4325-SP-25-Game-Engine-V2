--[[

This test script can be used to create a basic player in the game.

To set up:

1. Create a Sprite object
2. Create a Script object within the sprite
3. Select the script object and put this file's full path location into its Script property.
4. Ensure the Static property in the sprite is set to false to enable physics.
5. Test the game by selecting "Game > Run Game" in the window's top menu bar.

A platform should also be created under the sprite for it to stand on.

1. Create a Sprite object
2. Select the object and modify its size and position properties to be below the player.
3. Ensure the Static and Collidable properties are enabled.

]]

-- Load audio to use later
local bgmId = "bgm"
Audio.LoadSound(bgmId, "assets/audio/bgm/Darkling.wav", true, true)
local jumpId = "test_sfx"
Audio.LoadSound(jumpId, "assets/audio/sfx/test_sfx.wav", false, false)

-- Play background music
Audio.SetMasterVolume(0.5)
Audio.PlaySound(bgmId, 0.05)

local Direction = "Right"
local State = "Idle"
-- Below paths are only used as examples. You will need to edit the full paths based on your own system.
local RunLeftDir = "C:\\Users\\User\\Downloads\\Player\\RunLeft\\"
local RunRightDir = "C:\\Users\\User\\Downloads\\Player\\RunRight\\"
local IdleLeftDir = "C:\\Users\\User\\Downloads\\Player\\IdleLeft.png"
local IdleRightDir = "C:\\Users\\User\\Downloads\\Player\\IdleRight.png"

local RunLeftSprites = {"LeftRun1.png", "LeftRun2.png", "LeftRun3.png", "LeftRun4.png", "LeftRun5.png", "LeftRun6.png", "LeftRun7.png", "LeftRun8.png", "LeftRun9.png", "LeftRun10.png"}
local RunRightSprites = {"RightRun1.png", "RightRun2.png", "RightRun3.png", "RightRun4.png", "RightRun5.png", "RightRun6.png", "RightRun7.png", "RightRun8.png", "RightRun9.png", "RightRun10.png"}

-- Add artificial delay into code
function wait(seconds)
    local start = os.clock()
    while os.clock() - start < seconds do end
end

-- State variables
local CurrentSprite = 1
local MaxSprites = 10
local JumpCooldown = false

-- Set camera to focus on script's parent object
camera:FocusOnObject(script:GetParent())

-- Run loop to enable player functionality
while (script.enabled) do
	-- Sound reverb effects (Press 1-4 on your keyboard)
	if Input and Input.isKeyDown(Input.KeyCode.NUM_1) then
        Audio.SetEnvironment(Audio.EnvironmentType.NORMAL)
    end
    
    if Input and Input.isKeyDown(Input.KeyCode.NUM_2) then
        Audio.SetEnvironment(Audio.EnvironmentType.CAVE)
    end
    
    if Input and Input.isKeyDown(Input.KeyCode.NUM_3) then
        Audio.SetEnvironment(Audio.EnvironmentType.UNDERWATER)
    end
    
    if Input and Input.isKeyDown(Input.KeyCode.NUM_4) then
		Audio.SetEnvironment(Audio.EnvironmentType.LARGE_HALL)
    end

	-- Cycle through sprite images
	if CurrentSprite >= MaxSprites then
		CurrentSprite = 1
	else
		CurrentSprite = CurrentSprite + 1
	end

	-- Detect player input for movement
    if Input.isKeyDown(Input.KeyCode.A) then
        Physics.applyForce(script:GetParent():GetPhysicsBodyId(), -100, 0)
		Direction = "Left"
		State = "Running"
		script:GetParent():SetProperty("Texture", RunLeftDir..RunLeftSprites[CurrentSprite])
	elseif Input.isKeyDown(Input.KeyCode.D) then
		Physics.applyForce(script:GetParent():GetPhysicsBodyId(), 100, 0)
		Direction = "Right"
		State = "Running"
		script:GetParent():SetProperty("Texture", RunRightDir..RunRightSprites[CurrentSprite])
	else
		State = "Idle"
		if (Direction == "Right") then
			script:GetParent():SetProperty("Texture", IdleRightDir)
		else
			script:GetParent():SetProperty("Texture", IdleLeftDir)
		end
	end
	if Input.isKeyPressed(Input.KeyCode.SPACE) then
		if (not JumpCooldown) then
			JumpCooldown = true
			Physics.applyForce(script:GetParent():GetPhysicsBodyId(), 0, 400)
			Audio.PlaySound(jumpId, 1.0)
			JumpCooldown = false
		end
	end

	wait(0.05)
end