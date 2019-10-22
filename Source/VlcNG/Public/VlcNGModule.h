/* Copyright (C) 2018-2019 Blue Mountains GmbH. All Rights Reserved.
*/

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

VLCNG_API DECLARE_LOG_CATEGORY_EXTERN(LogVlc, Log, All);

/**
 * VlcNGModule interface
 */
class IVlcNGModule : public IModuleInterface
{
public:
	/**
	 * Get or load the VLC Module
	 * 
	 * @return The loaded module
	 */
	static inline IVlcNGModule& Get()
	{
		return FModuleManager::LoadModuleChecked< IVlcNGModule >("VlcNG");
	}
	
	/**
	 * Check whether the module has already been loaded
	 * 
	 * @return True if the module is loaded
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("VlcNG");
	}
};
