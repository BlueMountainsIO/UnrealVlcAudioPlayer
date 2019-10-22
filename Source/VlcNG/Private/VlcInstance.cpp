/* Copyright (C) 2018-2019 Blue Mountains GmbH. All Rights Reserved.
*/

#include "VlcInstance.h"
#include "VlcNGModule.h"

VlcInstance *VlcInstance::Instance = nullptr;

VlcInstance::VlcInstance()
{
	const char *Argv[] = {
		"--no-video",
		"--no-spu",
		"--no-stats",
		"--no-color",
		"--no-lua",
		"--no-interact",
		"--ignore-config",
		/*"--no-plugins-cache",*/
#if UE_BUILD_SHIPPING == 0
		"--verbose=2"
#else
		"--quiet"
#endif
	};

	LibVlcInstance = new VLC::Instance(sizeof(Argv) / sizeof(*Argv), Argv);

#if UE_BUILD_SHIPPING == 0
	LibVlcInstance->logSet2([](int lvl, const libvlc_log_t*, std::string message) {
		if (lvl == 4) {
			UE_LOG(LogVlc, Error, TEXT("%s"), ANSI_TO_TCHAR(message.c_str()));
		}
		else if (lvl == 3) {
			UE_LOG(LogVlc, Warning, TEXT("%s"), ANSI_TO_TCHAR(message.c_str()));
		}
		else if (lvl == 2) {
			UE_LOG(LogVlc, Log, TEXT("%s"), ANSI_TO_TCHAR(message.c_str()));
		}
		else {
			UE_LOG(LogVlc, Verbose, TEXT("%s"), ANSI_TO_TCHAR(message.c_str()));
		}
	});
#endif

	UE_LOG(LogVlc, Log, TEXT("libvlc Version %s-%s, %s"), ANSI_TO_TCHAR(libvlc_get_version()), ANSI_TO_TCHAR(libvlc_get_changeset()), ANSI_TO_TCHAR(libvlc_get_compiler()));
}

VlcInstance::~VlcInstance()
{
	if (LibVlcInstance != nullptr)
	{
		delete LibVlcInstance;
	}
}

VlcInstance *VlcInstance::Get()
{
	if (!Instance)
	{
		Instance = new VlcInstance();
	}

	return Instance;
}

shared_ptr<VlcAudioPlayer> VlcInstance::CreateAudioPlayer(const wstring &mrl)
{
	return make_shared<VlcAudioPlayer>(LibVlcInstance, std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(mrl));
}
