// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "GameFramework/Actor.h"
#include "Camera/CameraActor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "StaticMeshResources.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "RGBDCamera.generated.h"

UCLASS()
class UROBOVISION_API ARGBDCamera : public ACameraActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARGBDCamera();

	// Sets default values for this actor's properties
	virtual ~ARGBDCamera();

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

	// Create the server port on 0.0.0.0
	// By enabling this, you can connect to the server
	// via your 127.0.0.1 or your 'real' ip like 192....
	// Please be aware that binding to 0.0.0.0 will
	// allow connections from any network adapter
	// on your system
	UPROPERTY(EditAnywhere, Category = "RGB-D Settings")
	bool bBindToAnyIP;

	// Capture color image
	UPROPERTY(EditAnywhere, Category = "RGB-D Settings")
	bool bCaptureColorImage;

	// Capture depth image
	UPROPERTY(EditAnywhere, Category = "RGB-D Settings")
	bool bCaptureDepthImage;

	// Capture object mask image
	UPROPERTY(EditAnywhere, Category = "RGB-D Settings")
	bool bCaptureObjectMaskImage;

	// Call ColorAllObjects on every tick()
	// Usually, every actor in the World get assigned a unique color
	// for the object mask on BeginPlay().
	// Activating this flag will assign colors to all actors 
	// on every tick. This might be necessary if you dynamically add and remove
	// objects to the scene after BeginPlay()
  // Please note that when activating this flag, 
  // the Color Initialization is done only once in BeginPlay()
  // in conjunction with the ColorGenerationMaximumAmount variable
  // to generate enough colors for your use case of the whole duration of 
  // the runtime
	UPROPERTY(EditAnywhere, Category = "RGB-D Settings")
	bool bColorAllObjectsOnEveryTick;

	// Activate verbose output on the coloring-related methods 
	// to know which objects have been found and their assigned RGB colors
	UPROPERTY(EditAnywhere, Category = "RGB-D Settings")
	bool bColoringObjectsIsVerbose;

	// The number of object colors that should be generated 
  // when using ColorAllObjectsOnEveryTick.
  // This number must be higher than the amount of Actors you have in your world at any time.
	UPROPERTY(EditAnywhere, Category = "RGB-D Settings")
	int ColorGenerationMaximumAmount;

private:
	// Camera capture component for color images (RGB)
	USceneCaptureComponent2D* ColorImgCaptureComp;

	// Camera capture component for depth image
	USceneCaptureComponent2D* DepthImgCaptureComp;

	// Camera capture component for object mask
	USceneCaptureComponent2D* ObjectMaskImgCaptureComp;

	// Material instance to get the depth data
	UMaterialInstanceDynamic* MaterialDepthInstance;

	// Private data container
	class PrivateData;
	PrivateData* Priv;

	// Are the capture components active
	bool bCompActive;

	// Buffers for reading the data from the GPU
	TArray<FFloat16Color>  ImageDepth, ImageObject;

	float FrameTime, TimePassed;
	TArray<uint8> DataColor, DataDepth, DataObject;
	TArray<FColor>ImageColor, ObjectColors;
	TMap<FString, uint32> ObjectToColor;
	uint32 ColorsUsed;
	TArray<uint32> FreedColors;
	bool Running, Paused;

	void ShowFlagsBasicSetting(FEngineShowFlags &ShowFlags) const;
	void ShowFlagsLit(FEngineShowFlags &ShowFlags) const;
	void ShowFlagsPostProcess(FEngineShowFlags &ShowFlags) const;
	void ShowFlagsVertexColor(FEngineShowFlags &ShowFlags) const;
	void ReadImage(UTextureRenderTarget2D *RenderTarget, TArray<FFloat16Color> &ImageData) const;
        void ReadColorImage(UTextureRenderTarget2D *RenderTarget, TArray<FColor> &ImageData) const;
	void ToColorImage(const TArray<FFloat16Color> &ImageData, uint8 *Bytes) const;
        void ToColorRGBImage(const TArray<FColor> &ImageData, uint8 *Bytes) const;
	void ToDepthImage(const TArray<FFloat16Color> &ImageData, uint8 *Bytes) const;
	void StoreImage(const uint8 *ImageData, const uint32 Size, const char *Name) const;
	void GenerateColors(const uint32_t NumberOfColors);
	bool ColorObject(AActor *Actor, const FString &name);
	bool ColorAllObjects();
	void RemoveNonExistingActorsFromColorMap();
	void ProcessColor();
	void ProcessDepth();
	void ProcessObject();
	void SetVisibility(FEngineShowFlags& Target, FEngineShowFlags& Source) const;
};
