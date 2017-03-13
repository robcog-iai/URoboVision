#pragma once

#include "ModuleManager.h"
#include "Engine.h"

class FUVisionModule : public IModuleInterface
{
public:

  /** IModuleInterface implementation */
  virtual void StartupModule() override;
  virtual void ShutdownModule() override;
};

#include <string>

DECLARE_LOG_CATEGORY_EXTERN(UVisionLog, Log, All);

#define OUT_AUX(LEVEL, MSG, ...) UE_LOG(UVisionLog, LEVEL, TEXT("[%s][%d] " MSG), ##__VA_ARGS__)

#define OUT_DEBUG(MSG, ...) OUT_AUX(Display, MSG, ANSI_TO_TCHAR(__FUNCTION__), __LINE__, ##__VA_ARGS__)
#define OUT_INFO(MSG, ...)  OUT_AUX(Display, MSG, ANSI_TO_TCHAR(__FUNCTION__), __LINE__, ##__VA_ARGS__)
#define OUT_WARN(MSG, ...)  OUT_AUX(Warning, MSG, ANSI_TO_TCHAR(__FUNCTION__), __LINE__, ##__VA_ARGS__)
#define OUT_ERROR(MSG, ...) OUT_AUX(Error, MSG, ANSI_TO_TCHAR(__FUNCTION__), __LINE__, ##__VA_ARGS__)

