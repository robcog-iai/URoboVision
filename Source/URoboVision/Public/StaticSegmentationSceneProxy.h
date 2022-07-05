// Copyright 2019, Institute for Artificial Intelligence - University of Bremen
#pragma once
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Runtime/Engine/Public/MaterialShared.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/Engine/Public/Rendering/SkeletalMeshRenderData.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"



class FStaticSegmentationSceneProxy : public FStaticMeshSceneProxy
{
public:
	FMaterialRenderProxy* MaterialRenderProxy;

	FStaticSegmentationSceneProxy(UStaticMeshComponent* Component, bool bForceLODsShareStaticLighting, UMaterialInterface* SegmentationMID);
	virtual void GetDynamicMeshElements(
		const TArray < const FSceneView * > & Views,
		const FSceneViewFamily & ViewFamily,
		uint32 VisibilityMap,
		FMeshElementCollector & Collector) const override;
	virtual bool GetMeshElement
	(
		int32 LODIndex,
		int32 BatchIndex,
		int32 ElementIndex,
		uint8 InDepthPriorityGroup,
	#if (ENGINE_MINOR_VERSION >= 22 || ENGINE_MAJOR_VERSION == 5)
		bool bUseSelectionOutline,
	#else
		bool bUseSelectedMaterial,
		bool bUseHoveredMaterial,
	#endif
		bool bAllowPreCulledIndices,
		FMeshBatch & OutMeshBatch
	) const override;
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView * View) const override;
};
