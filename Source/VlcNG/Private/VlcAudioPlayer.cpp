/* Copyright (C) 2018-2019 Blue Mountains GmbH. All Rights Reserved.
*/

#include "VlcAudioPlayer.h"
#include "Runtime/Core/Public/Async/Async.h"

VlcAudioPlayer::VlcAudioPlayer()
{

}

VlcAudioPlayer::VlcAudioPlayer(VLC::Instance *instance, const string &mrl)
{
	static const string Http = "http://";
	static const string Https = "https://";

	if (mrl.compare(0, Http.length(), Http) == 0 ||
		mrl.compare(0, Https.length(), Https) == 0)
	{
		Media = new VLC::Media(*instance, mrl, VLC::Media::FromLocation);
	}
	else
	{
		string Backslash(mrl);
		std::replace(Backslash.begin(), Backslash.end(), '/', '\\');

		Media = new VLC::Media(*instance, Backslash, VLC::Media::FromPath);
	}

	if (!Media->isValid())
	{
		return;
	}

	MediaPlayer = new VLC::MediaPlayer(*Media);

	if (!MediaPlayer->isValid())
	{
		return;
	}

	MediaPlayer->setAudioFormatCallbacks([](char *format, unsigned *rate, unsigned *channels) -> int {
		memcpy(format, "S16N", 4);
		*rate = VLC_SAMPLE_RATE;
		*channels = VLC_CHANNELS;
		return 0;
	}, nullptr);

	MediaPlayer->setAudioCallbacks([this](const void *data, uint32_t count, int64_t pts) {
		const std::size_t SamplesSize = count * VLC_SAMPLE_SIZE * VLC_CHANNELS;

		if (SamplesSize > 4)
		{
			lock_guard<mutex> lock(PlayerMtx);

			AudioSamples.push(FVlcAudioSamples(data, SamplesSize));
		}
	}, nullptr, nullptr, nullptr, nullptr);

	SetupEvents();
}

VlcAudioPlayer::~VlcAudioPlayer()
{
	CleanupEvents();

	if (MediaPlayer != nullptr)
	{
		if (MediaPlayer->isPlaying())
		{
			MediaPlayer->stop();
		}

		delete MediaPlayer;
		MediaPlayer = nullptr;
	}

	if (Media != nullptr)
	{
		delete Media;
		Media = nullptr;
	}
}

void VlcAudioPlayer::SetupEvents()
{
	VlcEvent_onEncounteredError = MediaPlayer->eventManager().onEncounteredError([this]() {
		ExecuteEvent(OnError);
	});

	VlcEvent_onPlaying = MediaPlayer->eventManager().onPlaying([this]() {
		ExecuteEvent(OnPlaying);
	});

	VlcEvent_onStopped = MediaPlayer->eventManager().onStopped([this]() {
		ExecuteEvent(OnStopped);
	});
}

void VlcAudioPlayer::CleanupEvents()
{
	if (MediaPlayer != nullptr)
	{
		if (VlcEvent_onEncounteredError != nullptr)
		{
			MediaPlayer->eventManager().unregister(VlcEvent_onEncounteredError);
		}

		if (VlcEvent_onPlaying != nullptr)
		{
			MediaPlayer->eventManager().unregister(VlcEvent_onPlaying);
		}

		if (VlcEvent_onStopped != nullptr)
		{
			MediaPlayer->eventManager().unregister(VlcEvent_onStopped);
		}
	}
}

void VlcAudioPlayer::ExecuteEvent(shared_ptr<VlcCb_t> Event)
{
	if (Event != nullptr)
	{
		weak_ptr<VlcCb_t> WeakCb = Event;
		AsyncTask(ENamedThreads::GameThread, [WeakCb]()
		{
			if (!WeakCb.expired())
			{
				auto Callback = WeakCb.lock();

				if (Callback)
				{
					(*Callback)();
				}
			}
		});
	}
}

bool VlcAudioPlayer::GetAudio(FVlcAudioSamples *OutAudio)
{
	lock_guard<mutex> mtx(PlayerMtx);

	if (AudioSamples.empty())
	{
		return false;
	}

	FVlcAudioSamples &Sample = AudioSamples.front();

	*OutAudio = Sample;

	AudioSamples.pop();

	return true;
}

bool VlcAudioPlayer::Play()
{
	return MediaPlayer->play();
}

int VlcAudioPlayer::GetState()
{
	return static_cast<int>(MediaPlayer->state());
}

bool VlcAudioPlayer::IsValid()
{
	if (Media && MediaPlayer)
	{
		if (Media->isValid() && MediaPlayer->isValid())
		{
			return true;
		}
	}

	return false;
}
