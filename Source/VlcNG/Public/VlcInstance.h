/* Copyright (C) 2018-2019 Blue Mountains GmbH. All Rights Reserved.
*/

#pragma once

#include "CoreMinimal.h"
#include "vlcpp/vlc.hpp"
#include "VlcAudioPlayer.h"

class VLCNG_API VlcInstance
{
public:
	VlcInstance();
	~VlcInstance();

	VlcInstance(const VlcInstance&) = delete;
	VlcInstance& operator=(const VlcInstance&) = delete;

	static VlcInstance *Get();

	shared_ptr<VlcAudioPlayer> CreateAudioPlayer(const wstring &mrl);

private:
	static VlcInstance *Instance;

	VLC::Instance *LibVlcInstance = nullptr;
};
