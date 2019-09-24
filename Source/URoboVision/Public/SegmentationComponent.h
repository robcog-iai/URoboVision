// Copyright 2019, Institute for Artificial Intelligence - University of Bremen

#pragma once
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Public/SkeletalRenderPublic.h"
#include "SegmentationComponent.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class UROBOVISION_API USegmentationComponent : public UPrimitiveComponent
{
	GENERATED_BODY()
public:
	USegmentationComponent(const FObjectInitializer& ObjectInitializer);
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform & LocalToWorld) const override;
	void SetSegmentationColor(FColor SegmentationColor);
	FColor GetSegmentationColor();
	virtual void OnRegister() override;

private:
	UPROPERTY()
	UMaterial* SegmentationMaterial;
	UPROPERTY()
	UMaterialInstanceDynamic* SegmentationMID;
	FColor SegmentationColor;
	FPrimitiveSceneProxy* CreateSceneProxy(UStaticMeshComponent* StaticMeshComponent);
	FPrimitiveSceneProxy* CreateSceneProxy(USkeletalMeshComponent* SkeletalMeshComponent);
};
