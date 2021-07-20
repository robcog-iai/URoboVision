// Copyright 2019, Institute for Artificial Intelligence - University of Bremen
#pragma once
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Public/Rendering/SkeletalMeshRenderData.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Runtime/Engine/Public/MaterialShared.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/Engine/Public/Rendering/SkeletalMeshRenderData.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"

class FSkeletalSegmentationSceneProxy : public FSkeletalMeshSceneProxy
{
public:
	FSkeletalSegmentationSceneProxy(const USkinnedMeshComponent* Component, FSkeletalMeshRenderData* InSkeletalMeshRenderData, UMaterialInterface* SegmentationMID);
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView * View) const override;
};
