/* Copyright (C) 2018-2019 Blue Mountains GmbH. All Rights Reserved.
*/

#pragma once

#include "CoreMinimal.h"

struct VLCNG_API FVlcAudioSamples
{
public:
	FVlcAudioSamples() :
		BufferSize(0)
	{

	}

	FVlcAudioSamples(const void *InBuffer, std::size_t InSize)
	{
		SamplesBuffer.AddUninitialized(InSize);

		FMemory::Memcpy(SamplesBuffer.GetData(), (uint8*)InBuffer, InSize);

		BufferSize = InSize;
	}

	FVlcAudioSamples& operator=(const FVlcAudioSamples &Other)
	{
		if (this != &Other)
		{
			SamplesBuffer.Empty();

			SamplesBuffer.AddUninitialized(Other.BufferSize);

			FMemory::Memcpy(SamplesBuffer.GetData(), Other.SamplesBuffer.GetData(), Other.BufferSize);

			BufferSize = Other.BufferSize;
		}
		return *this;
	}

	~FVlcAudioSamples()
	{
		SamplesBuffer.Empty();
		BufferSize = 0;
	}

	TArray<uint8> SamplesBuffer;
	std::size_t BufferSize;
};
