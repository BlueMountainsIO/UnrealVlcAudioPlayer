https://wiki.videolan.org/GenerateLibFromDll/

libvlc needs to be delay loaded in UE4, use above steps to generate a new libvlc.lib because the original one is being ignored as delay loaded

.def file must start with “EXPORTS” and only contain the function names per line
With notepad++ Replace “^.*(libvlc)” with “libvlc” in Regular expression mode.

Download libvlc from:
http://download.videolan.org/pub/videolan/vlc/

Copy plugins/, libvlc.dll, libvlccore.dll to Engine/Binaries/ThirdParty/VLC/Win64
Copy include/, lib/ to Engine/Source/ThirdParty/VLC. Make sure to include your generated libvlc.lib

Makes use the of vlc C++ wrapper "libvlcpp". Keep in mind that VLC is licensed under GPL which is incompatible to Unreal Engine 4.

Example usage:
```C++
SoundWave = NewObject<USoundWaveProcedural>(USoundWaveProcedural::StaticClass());
SoundWave->SetSampleRate(VLC_SAMPLE_RATE);
SoundWave->NumChannels = VLC_CHANNELS;
SoundWave->Duration = INDEFINITELY_LOOPING_DURATION;
SoundWave->bLooping = false;
SoundWave->bCanProcessAsync = true;
#if ENGINE_MINOR_VERSION <= 22
SoundWave->bVirtualizeWhenSilent = true; // Play sound even when out of range
#else
SoundWave->VirtualizationMode = EVirtualizationMode::PlayWhenSilent;
#endif

auto AudioPlayer = VlcInstance::Get()->CreateAudioPlayer(File);

if (!AudioPlayer->IsValid())
{
	UE_LOG(LogTemp, Error, TEXT("AudioPlayer->IsValid() failed"));
	return false;
}

AudioPlayer->SetEvents(
	[this]() {
		UE_LOG(LogTemp, Warning, TEXT("VLC reports an error %i"), FPlatformTLS::GetCurrentThreadId());
	},
	[]() {
		UE_LOG(LogTemp, Warning, TEXT("VLC starts playing! %i"), FPlatformTLS::GetCurrentThreadId());
	},
	[this]() {
		UE_LOG(LogTemp, Warning, TEXT("VLC stopped. %i"), FPlatformTLS::GetCurrentThreadId());
	});

if (!AudioPlayer->Play())
{
	UE_LOG(LogTemp, Error, TEXT("AudioPlayer->Play() failed"));
	return false;
}

weak_ptr<VlcAudioPlayer> WeakPlayer = AudioPlayer;
SoundWave->OnSoundWaveProceduralUnderflow = FOnSoundWaveProceduralUnderflow::CreateLambda([WeakPlayer](USoundWaveProcedural* InProceduralWave, int32 SamplesRequired) {
	if (!WeakPlayer.expired())
	{
		auto AudioPlayer = WeakPlayer.lock();

		if (AudioPlayer)
		{
			FVlcAudioSamples AudioData;
			if (AudioPlayer->GetAudio(&AudioData))
			{
				UE_LOG(LogTemp, VeryVerbose, TEXT("SoundID Underflow requires %i samples, got %i"), SamplesRequired, AudioData.BufferSize);

				InProceduralWave->QueueAudio(AudioData.SamplesBuffer.GetData(), AudioData.BufferSize);
			}
		}
	}
});
```

