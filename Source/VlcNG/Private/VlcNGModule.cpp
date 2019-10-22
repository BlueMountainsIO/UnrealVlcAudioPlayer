/* Copyright (C) 2018-2019 Blue Mountains GmbH. All Rights Reserved.
*/

#include "VlcNGModule.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "HAL/PlatformProcess.h"

DEFINE_LOG_CATEGORY(LogVlc);

class FVlcNGModule : public IVlcNGModule
{
private:
	// IModuleInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE( FVlcNGModule, VlcNG );

void* LoadDllVLC(const FString& Path)
{
	if (Path.IsEmpty())
	{
		return nullptr;
	}
	void* Handle = FPlatformProcess::GetDllHandle(*Path);
	if (!Handle)
	{
		int32 ErrorNum = FPlatformMisc::GetLastError();
		TCHAR ErrorMsg[1024];
		FPlatformMisc::GetSystemErrorMessage(ErrorMsg, 1024, ErrorNum);
		UE_LOG(LogVlc, Error, TEXT("Failed to get VLC DLL handle for %s: %s (%d)"), *Path, ErrorMsg, ErrorNum);
	}
	return Handle;
}

void* VLCHandle = nullptr;

void FVlcNGModule::StartupModule()
{
	//UE_LOG(LogTemp, Warning, TEXT("FVlcNGModule::StartupModule"));

	FString DllPath(FPaths::Combine(*FPaths::EngineDir(), TEXT("Binaries/ThirdParty/VLC/Win64")));

	FPlatformProcess::PushDllDirectory(*DllPath);
	VLCHandle = LoadDllVLC(FPaths::Combine(*DllPath, TEXT("libvlc.dll")));
	FPlatformProcess::PopDllDirectory(*DllPath);

	// Keep directory for later dll loading in plugins/
	FPlatformProcess::AddDllDirectory(*DllPath);
}

void FVlcNGModule::ShutdownModule()
{
	//UE_LOG(LogTemp, Warning, TEXT("FVlcNGModule::ShutdownModule"));

	FPlatformProcess::FreeDllHandle(VLCHandle);
	VLCHandle = nullptr;
}
