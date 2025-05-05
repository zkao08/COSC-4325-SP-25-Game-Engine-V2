local bgmId = "bgm"
Audio.LoadSound(bgmId, "assets/audio/bgm/Darkling.wav", true, true)
local jumpId = "test_sfx"
Audio.LoadSound(jumpId, "assets/audio/sfx/test_sfx.wav", false, false)

Audio.SetMasterVolume(0.5)
Audio.PlaySound(bgmId, 0.05)

local Direction = "Right"
local State = "Idle"
local RunLeftDir = "C:\\Users\\Voxel\\Downloads\\Player\\RunLeft\\"
local RunRightDir = "C:\\Users\\Voxel\\Downloads\\Player\\RunRight\\"
local IdleLeftDir = "C:\\Users\\Voxel\\Downloads\\Player\\IdleLeft.png"
local IdleRightDir = "C:\\Users\\Voxel\\Downloads\\Player\\IdleRight.png"

local RunLeftSprites = {"LeftRun1.png", "LeftRun2.png", "LeftRun3.png", "LeftRun4.png", "LeftRun5.png", "LeftRun6.png", "LeftRun7.png", "LeftRun8.png", "LeftRun9.png", "LeftRun10.png"}

local RunRightSprites = {"RightRun1.png", "RightRun2.png", "RightRun3.png", "RightRun4.png", "RightRun5.png", "RightRun6.png", "RightRun7.png", "RightRun8.png", "RightRun9.png", "RightRun10.png"}

function wait(seconds)
    local start = os.clock()  -- Get the current time
    while os.clock() - start < seconds do end
end

local CurrentSprite = 1
local MaxSprites = 10
local JumpCooldown = false

camera:FocusOnObject(script:GetParent())

while (script.enabled) do
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

	if CurrentSprite >= MaxSprites then
		CurrentSprite = 1
	else
		CurrentSprite = CurrentSprite + 1
	end
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