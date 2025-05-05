local Direction = "Right"
local State = "Idle"
local RunLeftDir = "%userprofile%\\Downloads\\PlayerSprites\\RunLeft\\"
local RunRightDir = "%userprofile%\\Downloads\\PlayerSprites\\RunRight\\"
local IdleLeftDir = "%userprofile%\\Downloads\\PlayerSprites\\IdleLeft.png"
local IdleRightDir = "%userprofile%\\Downloads\\PlayerSprites\\IdleRight.png"

local RunLeftSprites = {"LeftRun1.png", "LeftRun2.png", "LeftRun3.png", "LeftRun4.png", "LeftRun5.png", "LeftRun6.png", "LeftRun7.png", "LeftRun8.png", "LeftRun9.png", "LeftRun10.png"}

local RunRightSprites = {"RightRun1.png", "RightRun2.png", "RightRun3.png", "RightRun4.png", "RightRun5.png", "RightRun6.png", "RightRun7.png", "RightRun8.png", "RightRun9.png", "RightRun10.png"}

function wait(seconds)
    local start = os.clock()  -- Get the current time
    while os.clock() - start < seconds do end
end

local CurrentSprite = 1
local MaxSprites = 10

while (script.enabled) do
	if CurrentSprite >= MaxSprites then
		CurrentSprite = 1
	else
		CurrentSprite = CurrentSprite + 1
	end
    if Input.isKeyDown(Input.KeyCode.A) then
        Physics.applyForce(script:GetParent():GetPhysicsBodyId(), -10, 0)
		Direction = "Left"
		State = "Running"
		script:GetParent():SetProperty("Texture", RunLeftDir..RunLeftSprites[CurrentSprite])
	elseif Input.isKeyDown(Input.KeyCode.D) then
		Physics.applyForce(script:GetParent():GetPhysicsBodyId(), 10, 0)
		Direction = "Right"
		State = "Running"
		print("RAN")
		script:GetParent():SetProperty("Texture", RunRightDir..RunRightSprites[CurrentSprite])
	else
		State = "Idle"
		if (Direction == "Right") then
			script:GetParent():SetProperty("Texture", IdleRightDir)
		else
			script:GetParent():SetProperty("Texture", IdleLeftDir)
		end
	end
	wait(0.05)
end