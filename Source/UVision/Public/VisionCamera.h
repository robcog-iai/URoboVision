#pragma once

#include "GameFramework/Actor.h"
#include "Camera/CameraActor.h"
#include "StaticMeshResources.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "VisionCamera.generated.h"

UCLASS()
class UVISION_API AVisionCamera : public ACameraActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVisionCamera();

	// Sets default values for this actor's properties
	virtual ~AVisionCamera();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the game ends
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Change the framerate on the fly
	void SetFramerate(const float NewFramerate);
  
	// Pause/resume camera
	void Pause(const bool NewPause = true);
  
	// Check if paused
	bool IsPaused() const;

	// Camera Width
	UPROPERTY(EditAnywhere, Category = "RGB-D Settings")
	uint32 Width;

	// Camera Heigth
	UPROPERTY(EditAnywhere, Category = "RGB-D Settings")
	uint32 Height;

	// Camera field of view
	UPROPERTY(EditAnywhere, Category = "RGB-D Settings")
	float FieldOfView;

	// Camera update rate
	UPROPERTY(EditAnywhere, Category = "RGB-D Settings")
	float Framerate;

	// Socket port for sending the data
	UPROPERTY(EditAnywhere, Category = "RGB-D Settings")
	int32 ServerPort;

private:
	// Private data container
	class PrivateData;
	PrivateData* Priv;

	// Camera capture component for color images (RGB)
	USceneCaptureComponent2D* ColorImgCaptureComp;

	// Camera capture component for depth image
	USceneCaptureComponent2D* DepthImgCaptureComp;

	// Camera capture component for object mask
	USceneCaptureComponent2D* ObjectMaskImgCaptureComp;

	// Material instance to get the depth data
	UMaterialInstanceDynamic* MaterialDepthInstance;

	float FrameTime, TimePassed;
	TArray<FFloat16Color> ImageColor, ImageDepth, ImageObject;
	TArray<uint8> DataColor, DataDepth, DataObject;
	TArray<FColor> ObjectColors;
	TMap<FString, uint32> ObjectToColor;
	uint32 ColorsUsed;
	bool Running, Paused;

	void ShowFlagsBasicSetting(FEngineShowFlags &ShowFlags) const;
	void ShowFlagsLit(FEngineShowFlags &ShowFlags) const;
	void ShowFlagsPostProcess(FEngineShowFlags &ShowFlags) const;
	void ShowFlagsVertexColor(FEngineShowFlags &ShowFlags) const;
	void ReadImage(UTextureRenderTarget2D *RenderTarget, TArray<FFloat16Color> &ImageData) const;
	void ToColorImage(const TArray<FFloat16Color> &ImageData, uint8 *Bytes) const;
	void ToDepthImage(const TArray<FFloat16Color> &ImageData, uint8 *Bytes) const;
	void StoreImage(const uint8 *ImageData, const uint32 Size, const char *Name) const;
	void GenerateColors(const uint32_t NumberOfColors);
	bool ColorObject(AActor *Actor, const FString &name);
	bool ColorAllObjects();
	void ProcessColor();
	void ProcessDepth();
	void ProcessObject();
};
