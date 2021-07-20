// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FURoboVisionModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#include <string>

DECLARE_LOG_CATEGORY_EXTERN(URoboVisionLog, Log, All);

#define OUT_AUX(LEVEL, MSG, ...) UE_LOG(URoboVisionLog, LEVEL, TEXT("[%s][%d] " MSG), ##__VA_ARGS__)

#define OUT_DEBUG(MSG, ...) OUT_AUX(Display, MSG, ANSI_TO_TCHAR(__FUNCTION__), __LINE__, ##__VA_ARGS__)
#define OUT_INFO(MSG, ...)  OUT_AUX(Display, MSG, ANSI_TO_TCHAR(__FUNCTION__), __LINE__, ##__VA_ARGS__)
#define OUT_WARN(MSG, ...)  OUT_AUX(Warning, MSG, ANSI_TO_TCHAR(__FUNCTION__), __LINE__, ##__VA_ARGS__)
#define OUT_ERROR(MSG, ...) OUT_AUX(Error, MSG, ANSI_TO_TCHAR(__FUNCTION__), __LINE__, ##__VA_ARGS__)

