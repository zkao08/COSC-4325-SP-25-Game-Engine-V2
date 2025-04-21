-- Audio Lua API Test Script
print("=== Audio Lua API Test Script ===")

-- Helper function for sleeping in Windows
-- Uses a simple busy-wait approach instead of external commands
function sleep(seconds)
  local start = os.time()
  -- Busy wait until enough time passes
  while os.time() < start + seconds do
    -- Do nothing, just wait
  end
end

-- Test audio resources
print("\n--- Testing SoundResource Creation ---")
local sfxResource = SoundResource.new("assets/audio/sfx/test_sfx.wav", Audio.ResourceType.SOUND_EFFECT)
print("Created SFX resource: " .. sfxResource:GetFilePath())
print("Is streaming: " .. tostring(sfxResource:IsStreaming()))

local bgmResource = SoundResource.new("assets/audio/bgm/test_bgm.wav", Audio.ResourceType.STREAMING)
print("Created BGM resource: " .. bgmResource:GetFilePath())
print("Is streaming: " .. tostring(bgmResource:IsStreaming()))

-- Test basic playback
print("\n--- Testing Basic Sound Playback ---")
print("Setting master volume to 70%")
Audio.SetMasterVolume(0.7)

print("Playing sound effect...")
local result = Audio.PlaySound("assets/audio/sfx/test_sfx.wav", true, 1.0)
print("PlaySound result: " .. tostring(result))

-- Wait for sound to play
print("Wait 2 seconds...")
sleep(2)

-- Test volume control
print("\n--- Testing Volume Control ---")
print("Playing sound at different volumes...")

print("Playing at 100% volume")
Audio.PlaySound("assets/audio/sfx/test_sfx.wav", true, 1.0)
sleep(1)

print("Playing at 60% volume")
Audio.PlaySound("assets/audio/sfx/test_sfx.wav", true, 0.6)
sleep(1)

print("Playing at 30% volume")
Audio.PlaySound("assets/audio/sfx/test_sfx.wav", true, 0.3)
sleep(1)

-- Test streaming audio
print("\n--- Testing Streaming Audio ---")
print("Playing streaming BGM at 50% volume...")
local bgmResult = Audio.PlaySound("assets/audio/bgm/test_bgm.wav", false, 0.5)
print("BGM PlaySound result: " .. tostring(bgmResult))

print("Wait 3 seconds...")
sleep(3)

print("Changing BGM volume to 80%")
Audio.SetSoundVolume("assets/audio/bgm/test_bgm.wav", 0.8)
sleep(3)

print("Changing BGM volume to 20%")
Audio.SetSoundVolume("assets/audio/bgm/test_bgm.wav", 0.2)
sleep(3)

-- Test master volume
print("\n--- Testing Master Volume ---")
print("Changing master volume to 100%")
Audio.SetMasterVolume(1.0)
sleep(2)

print("Changing master volume to 30%")
Audio.SetMasterVolume(0.3)
sleep(2)

print("Changing master volume back to 70%")
Audio.SetMasterVolume(0.7)
sleep(2)

-- Test Environment Types
print("\n--- Testing Environment Types ---")
print("Changing environment to CAVE")
Audio.SetEnvironment(Audio.EnvironmentType.CAVE)
sleep(3)

print("Changing environment to UNDERWATER")
Audio.SetEnvironment(Audio.EnvironmentType.UNDERWATER)
sleep(3)

print("Changing environment to LARGE_HALL")
Audio.SetEnvironment(Audio.EnvironmentType.LARGE_HALL)
sleep(3)

print("Changing environment back to NORMAL")
Audio.SetEnvironment(Audio.EnvironmentType.NORMAL)
sleep(3)

-- Test sound stopping
print("\n--- Testing Sound Stopping ---")
print("Playing sound effect...")
Audio.PlaySound("assets/audio/sfx/test_sfx.wav")
sleep(1)

print("Stopping sound effect")
Audio.StopSound("assets/audio/sfx/test_sfx.wav")
sleep(1)

print("Stopping BGM")
Audio.StopSound("assets/audio/bgm/test_bgm.wav")
sleep(2)

-- Test memory management
print("\n--- Testing Memory Management ---")
print("Configuring cache (max 10 resources, 2s min age)")
Audio.ConfigureCache(10, 2)

-- Load multiple sounds to test cache behavior
print("\nLoading multiple sounds...")
for i = 1, 15 do
  local soundPath = "assets/audio/sfx/test_sfx_" .. i .. ".wav"
  print("Loading sound: " .. soundPath)
  Audio.PlaySound(soundPath, true, 0.1)
  -- Very short delay to avoid hanging
  sleep(0.5)
end

print("\nWaiting for resources to age...")
sleep(3)

-- Adding back the CleanupResourceCache calls
print("\nManually triggering cache cleanup to configured maximum:")
pcall(function() 
  -- Use pcall to prevent script termination if an error occurs
  Audio.CleanupResourceCache()
  print("Cleanup completed successfully")
end)
sleep(1)

print("\nDemonstrating aggressive cleanup to 5 resources:")
pcall(function()
  -- Use pcall to prevent script termination if an error occurs 
  Audio.CleanupResourceCache(5)
  print("Aggressive cleanup completed successfully")
end)

print("\nTest complete. Stopping all sounds")
Audio.StopAllSounds()

print("\n=== Audio API Test Complete ===")