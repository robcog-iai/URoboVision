// Copyright 2019, Institute for Artificial Intelligence - University of Bremen

#include "StaticSegmentationSceneProxy.h"

FStaticSegmentationSceneProxy::FStaticSegmentationSceneProxy(UStaticMeshComponent* Component, bool bForceLODsShareStaticLighting, UMaterialInterface* SegmentationMID) :
		FStaticMeshSceneProxy(Component, bForceLODsShareStaticLighting)
	{
	#if (ENGINE_MINOR_VERSION >= 22 || ENGINE_MAJOR_VERSION == 5)
		MaterialRenderProxy = SegmentationMID->GetRenderProxy();
	#else
		MaterialRenderProxy = SegmentationMID->GetRenderProxy(false, false);
	#endif
		this->MaterialRelevance = SegmentationMID->GetRelevance(GetScene().GetFeatureLevel());
		this->bVerifyUsedMaterials = false;
		bCastShadow = false;
	}

FPrimitiveViewRelevance FStaticSegmentationSceneProxy::GetViewRelevance(const FSceneView * View) const
{
	if (View->Family->EngineShowFlags.Materials)
	{
		FPrimitiveViewRelevance ViewRelevance;
		ViewRelevance.bDrawRelevance = 0;
		return ViewRelevance;
	}
	else
	{
		return FStaticMeshSceneProxy::GetViewRelevance(View);
	}
}


void FStaticSegmentationSceneProxy::GetDynamicMeshElements(
	const TArray < const FSceneView * > & Views,
	const FSceneViewFamily & ViewFamily,
	uint32 VisibilityMap,
	FMeshElementCollector & Collector) const
{
	FStaticMeshSceneProxy::GetDynamicMeshElements(Views, ViewFamily, VisibilityMap, Collector);
}

bool FStaticSegmentationSceneProxy::GetMeshElement(
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
	FMeshBatch & OutMeshBatch) const
{
	bool Ret = FStaticMeshSceneProxy::GetMeshElement(LODIndex, BatchIndex, ElementIndex, InDepthPriorityGroup,
#if (ENGINE_MINOR_VERSION >= 22 || ENGINE_MAJOR_VERSION == 5)
	bUseSelectionOutline,
#else
	bUseSelectedMaterial,
	bUseHoveredMaterial,
#endif
	bAllowPreCulledIndices, OutMeshBatch);
	OutMeshBatch.MaterialRenderProxy = this->MaterialRenderProxy;
	return Ret;
}
