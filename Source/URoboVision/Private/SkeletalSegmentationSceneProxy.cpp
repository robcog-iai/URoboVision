// Copyright 2019, Institute for Artificial Intelligence - University of Bremen

#include "SkeletalSegmentationSceneProxy.h"

FSkeletalSegmentationSceneProxy::FSkeletalSegmentationSceneProxy(const USkinnedMeshComponent* Component, FSkeletalMeshRenderData* InSkeletalMeshRenderData, UMaterialInterface* SegmentationMID)
	: FSkeletalMeshSceneProxy(Component, InSkeletalMeshRenderData)
	{
		this->bVerifyUsedMaterials = false;
		this->bCastDynamicShadow = false;
		for(int32 LODIdx=0; LODIdx < LODSections.Num(); LODIdx++)
		{
			FLODSectionElements& LODSection = LODSections[LODIdx];
			for(int32 SectionIndex = 0; SectionIndex < LODSection.SectionElements.Num(); SectionIndex++)
			{
				if (IsValid(SegmentationMID))
				{
					LODSection.SectionElements[SectionIndex].Material = SegmentationMID;
				}
				else
				{
					OUT_INFO(TEXT("SegmentationMaterial is Invalid in FSkeletalSceneProxy"));
				}
			}
		}
	}

FPrimitiveViewRelevance FSkeletalSegmentationSceneProxy::GetViewRelevance(const FSceneView * View) const
{
	if (View->Family->EngineShowFlags.Materials)
	{
		FPrimitiveViewRelevance ViewRelevance;
		ViewRelevance.bDrawRelevance = 0;
		return ViewRelevance;
	}
	else
	{
		return FSkeletalMeshSceneProxy::GetViewRelevance(View);
	}
}

