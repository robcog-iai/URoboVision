// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UVision.h"
#include "UVisionPrivatePCH.h"

#define LOCTEXT_NAMESPACE "FUVisionModule"

void FUVisionModule::StartupModule()
{
  // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FUVisionModule::ShutdownModule()
{
  // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
  // we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

DEFINE_LOG_CATEGORY(UVisionLog);

IMPLEMENT_MODULE(FUVisionModule, UVision)
