/* Copyright (C) 2018-2019 Blue Mountains GmbH. All Rights Reserved.
*/

#pragma once

#include "CoreMinimal.h"
#include "VlcAudioSamples.h"
#include "vlcpp/vlc.hpp"

#include <memory>
#include <functional>
#include <mutex>
#include <string>
#include <codecvt>
#include <queue>
#include <algorithm>

using std::shared_ptr;
using std::unique_ptr;
using std::weak_ptr;
using std::make_shared;
using std::make_unique;
using std::function;
using std::mutex;
using std::lock_guard;
using std::string;
using std::wstring;
using std::queue;

#define VLC_SAMPLE_RATE 44100
#define VLC_CHANNELS 2
#define VLC_SAMPLE_SIZE 2 // Using S16N

class VLCNG_API VlcAudioPlayer
{
public:
	typedef function<void()> VlcCb_t;

	VlcAudioPlayer();

	VlcAudioPlayer(VLC::Instance *instance, const string &mrl);

	~VlcAudioPlayer();

	template <typename ErrorCb, typename PlayingCb, typename StoppedCb>
	void SetEvents(ErrorCb &&error, PlayingCb &&playing, StoppedCb &&stopped)
	{
		OnError = make_shared<VlcCb_t>(error);
		OnPlaying = make_shared<VlcCb_t>(playing);
		OnStopped = make_shared<VlcCb_t>(stopped);
	}

	bool GetAudio(FVlcAudioSamples *OutAudio);

	bool Play();

	int GetState();

	bool IsValid();

private:
	shared_ptr<VlcCb_t> OnError;
	shared_ptr<VlcCb_t> OnPlaying;
	shared_ptr<VlcCb_t> OnStopped;

	VLC::Media *Media = nullptr;
	VLC::MediaPlayer *MediaPlayer = nullptr;

	mutex PlayerMtx;
	queue<FVlcAudioSamples> AudioSamples;

	VLC::EventManager::RegisteredEvent VlcEvent_onEncounteredError = nullptr;
	VLC::EventManager::RegisteredEvent VlcEvent_onPlaying = nullptr;
	VLC::EventManager::RegisteredEvent VlcEvent_onStopped = nullptr;

	void SetupEvents();
	void CleanupEvents();

	void ExecuteEvent(shared_ptr<VlcCb_t> Event);
};
