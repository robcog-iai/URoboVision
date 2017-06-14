// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#include "RGBDCamera.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Camera/CameraComponent.h"
#include "ConstructorHelpers.h"
#include "EngineUtils.h"
#include "StopTime.h"
#include "Server.h"
#include "PacketBuffer.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <cmath>
#include <condition_variable>


// Private data container so that internal structures are not visible to the outside
class UROBOVISION_API ARGBDCamera::PrivateData
{
public:
	TSharedPtr<PacketBuffer> Buffer;
	TCPServer Server;
	std::mutex WaitColor, WaitDepth, WaitObject, WaitDone;
	std::condition_variable CVColor, CVDepth, CVObject, CVDone;
	std::thread ThreadColor, ThreadDepth, ThreadObject;
	bool DoColor, DoDepth, DoObject;
	bool DoneColor, DoneObject;
};

// Sets default values
ARGBDCamera::ARGBDCamera() /*: ACameraActor(), Width(960), Height(540), Framerate(1), FieldOfView(90.0), ServerPort(10000), FrameTime(1.0f / Framerate), TimePassed(0), ColorsUsed(0)*/
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Default vison camera values
	Width = 960;
	Height = 540;
	Framerate = 1.f;
	FieldOfView = 90.f;
	FrameTime = 1.0f / Framerate;
	TimePassed = 0.f;
	ColorsUsed = 0;

	// Set FOV and aspect ratio
	GetCameraComponent()->FieldOfView = FieldOfView;
	GetCameraComponent()->AspectRatio = Width / (float)Height;

	// Create the vision capture components
	ColorImgCaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("ColorCapture"));
	ColorImgCaptureComp->SetupAttachment(RootComponent);
	ColorImgCaptureComp->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	ColorImgCaptureComp->TextureTarget = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("ColorTarget"));
	ColorImgCaptureComp->TextureTarget->InitAutoFormat(Width, Height);
	ColorImgCaptureComp->FOVAngle = FieldOfView;
	//Color->TextureTarget->TargetGamma = GEngine->GetDisplayGamma();

	DepthImgCaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("DepthCapture"));
	DepthImgCaptureComp->SetupAttachment(RootComponent);
	DepthImgCaptureComp->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	DepthImgCaptureComp->TextureTarget = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("DepthTarget"));
	DepthImgCaptureComp->TextureTarget->InitAutoFormat(Width, Height);
	DepthImgCaptureComp->FOVAngle = FieldOfView;

	ObjectMaskImgCaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("ObjectCapture"));
	ObjectMaskImgCaptureComp->SetupAttachment(RootComponent);
	ObjectMaskImgCaptureComp->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	ObjectMaskImgCaptureComp->TextureTarget = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("ObjectTarget"));
	ObjectMaskImgCaptureComp->TextureTarget->InitAutoFormat(Width, Height);
	ObjectMaskImgCaptureComp->FOVAngle = FieldOfView;

	// Disable the capture components by default (enable if needed in begin play)
	ColorImgCaptureComp->SetHiddenInGame(true);
	ColorImgCaptureComp->Deactivate();
	DepthImgCaptureComp->SetHiddenInGame(true);
	DepthImgCaptureComp->Deactivate();
	ObjectMaskImgCaptureComp->SetHiddenInGame(true);
	ObjectMaskImgCaptureComp->Deactivate();
	bCompActive = false;

	// Capture flags default values
	bCaptureColorImage = true;
	bCaptureDepthImage = true;
	bCaptureObjectMaskImage = true;
	
	// TCP IP communication server port
	ServerPort = 10000;

	// Setting flags for each camera
	ShowFlagsLit(ColorImgCaptureComp->ShowFlags);
	ShowFlagsPostProcess(DepthImgCaptureComp->ShowFlags);
	ShowFlagsVertexColor(ObjectMaskImgCaptureComp->ShowFlags);

	// Get depth scene material for postprocessing
	OUT_INFO(TEXT("Loading materials."));
	ConstructorHelpers::FObjectFinder<UMaterial> MaterialDepthFinder(TEXT("Material'/URoboVision/SceneDepth.SceneDepth'"));
	if(MaterialDepthFinder.Object != nullptr)
	{
		MaterialDepthInstance = UMaterialInstanceDynamic::Create(MaterialDepthFinder.Object, DepthImgCaptureComp);
		if(MaterialDepthInstance != nullptr)
		{
			DepthImgCaptureComp->PostProcessSettings.AddBlendable(MaterialDepthInstance, 1);
		}
	}
	else
	{
		OUT_ERROR(TEXT("Could not load material for depth."));
	}

	// Initializing buffers for reading images from the GPU
	ImageColor.AddUninitialized(Width * Height);
	ImageDepth.AddUninitialized(Width * Height);
	ImageObject.AddUninitialized(Width * Height);

	// Creating double buffer and setting the pointer of the server object
	Priv = new PrivateData();
	Priv->Buffer = TSharedPtr<PacketBuffer>(new PacketBuffer(Width, Height, FieldOfView));
	Priv->Server.Buffer = Priv->Buffer;
}

ARGBDCamera::~ARGBDCamera()
{
	delete Priv;
	OUT_INFO(TEXT("RGBDCamera got destroyed!"));
}

// Called when the game starts or when spawned
void ARGBDCamera::BeginPlay()
{
	Super::BeginPlay();
	OUT_INFO(TEXT("Begin play!"));

	// Starting server
	Priv->Server.Start(ServerPort);

	// Coloring all objects
	ColorAllObjects();

	Running = true;
	Paused = false;

	Priv->DoColor = false;
	Priv->DoObject = false;
	Priv->DoDepth = false;

	Priv->DoneColor = false;
	Priv->DoneObject = false;

	// Starting threads to process image data
	if (bCaptureColorImage)
	{
		ColorImgCaptureComp->SetHiddenInGame(false);
		ColorImgCaptureComp->Activate();
		bCompActive = true;
		Priv->ThreadColor = std::thread(&ARGBDCamera::ProcessColor, this);
	}
	if (bCaptureDepthImage)
	{
		DepthImgCaptureComp->SetHiddenInGame(false);
		DepthImgCaptureComp->Activate();
		bCompActive = true;
		Priv->ThreadDepth = std::thread(&ARGBDCamera::ProcessDepth, this);
	}
	if (bCaptureObjectMaskImage)
	{
		ObjectMaskImgCaptureComp->SetHiddenInGame(false);
		ObjectMaskImgCaptureComp->Activate();
		bCompActive = true;
		Priv->ThreadObject = std::thread(&ARGBDCamera::ProcessObject, this);
	}
}

// Called when the game starts or when spawned
void ARGBDCamera::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	OUT_INFO(TEXT("End play!"));

	Running = false;

	// Stopping processing threads
	Priv->DoColor = true;
	Priv->DoDepth = true;
	Priv->DoObject = true;
	Priv->CVColor.notify_one();
	Priv->CVDepth.notify_one();
	Priv->CVObject.notify_one();

	Priv->ThreadColor.join();
	Priv->ThreadDepth.join();
	Priv->ThreadObject.join();

	Priv->Server.Stop();
}

// Called every frame
void ARGBDCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check if paused
	if(Paused)
	{
		return;
	}

	// Check for framerate
	TimePassed += DeltaTime;
	if(TimePassed < 1.0f / Framerate)
	{
		return;
	}
	TimePassed -= 1.0f / Framerate;
	//MEASURE_TIME("Tick");
	//OUT_INFO(TEXT("FRAME_RATE: %f"),Framerate)

	UpdateComponentTransforms();

	// Check if client is connected
	if(!Priv->Server.HasClient())
	{
		return;
	}

	FDateTime Now = FDateTime::UtcNow();
	Priv->Buffer->HeaderWrite->TimestampCapture = Now.ToUnixTimestamp() * 1000000000 + Now.GetMillisecond() * 1000000;

	FVector Translation = GetActorLocation();
	FQuat Rotation = GetActorQuat();
	// Convert to meters and ROS coordinate system
	Priv->Buffer->HeaderWrite->Translation.X = Translation.X / 100.0f;
	Priv->Buffer->HeaderWrite->Translation.Y = -Translation.Y / 100.0f;
	Priv->Buffer->HeaderWrite->Translation.Z = Translation.Z / 100.0f;
	Priv->Buffer->HeaderWrite->Rotation.X = -Rotation.X;
	Priv->Buffer->HeaderWrite->Rotation.Y = Rotation.Y;
	Priv->Buffer->HeaderWrite->Rotation.Z = -Rotation.Z;
	Priv->Buffer->HeaderWrite->Rotation.W = Rotation.W;

	// Start writing to buffer
	Priv->Buffer->StartWriting(ObjectToColor, ObjectColors);

	// Read color image and notify processing thread
	Priv->WaitColor.lock();
	ReadImage(ColorImgCaptureComp->TextureTarget, ImageColor);
	Priv->WaitColor.unlock();
	Priv->DoColor = true;
	Priv->CVColor.notify_one();

	// Read object image and notify processing thread
	Priv->WaitObject.lock();
	ReadImage(ObjectMaskImgCaptureComp->TextureTarget, ImageObject);
	Priv->WaitObject.unlock();
	Priv->DoObject = true;
	Priv->CVObject.notify_one();

	/* Read depth image and notify processing thread. Depth processing is called last,
	* because the color image processing thread take more time so they can already begin.
	* The depth processing thread will wait for the others to be finished and then releases
	* the buffer.
	*/
	Priv->WaitDepth.lock();
	ReadImage(DepthImgCaptureComp->TextureTarget, ImageDepth);
	Priv->WaitDepth.unlock();
	Priv->DoDepth = true;
	Priv->CVDepth.notify_one();
}

void ARGBDCamera::SetFramerate(const float _Framerate)
{
	Framerate = _Framerate;
	FrameTime = 1.0f / _Framerate;
	TimePassed = 0;
	OUT_INFO(TEXT("FRAMERATE SET TO: %f"),Framerate);
}

void ARGBDCamera::Pause(const bool _Pause)
{
	Paused = _Pause;
	if (Paused)
	{
		// Disable the capture components by default (enable if needed in begin play)
		ColorImgCaptureComp->SetHiddenInGame(true);
		ColorImgCaptureComp->Deactivate();
		DepthImgCaptureComp->SetHiddenInGame(true);
		DepthImgCaptureComp->Deactivate();
		ObjectMaskImgCaptureComp->SetHiddenInGame(true);
		ObjectMaskImgCaptureComp->Deactivate();
		bCompActive = false;	
	}
	else
	{
		// Enable capture 
		if (bCaptureColorImage)
		{
			ColorImgCaptureComp->SetHiddenInGame(false);
			ColorImgCaptureComp->Activate();

		}
		if (bCaptureDepthImage)
		{
			DepthImgCaptureComp->SetHiddenInGame(false);
			DepthImgCaptureComp->Activate();
		}
		if (bCaptureObjectMaskImage)
		{
			ObjectMaskImgCaptureComp->SetHiddenInGame(false);
			ObjectMaskImgCaptureComp->Activate();
		}
		bCompActive = true;
	}
}

bool ARGBDCamera::IsPaused() const
{
	return Paused;
}

void ARGBDCamera::ShowFlagsBasicSetting(FEngineShowFlags &ShowFlags) const
{
	ShowFlags = FEngineShowFlags(EShowFlagInitMode::ESFIM_All0);
	ShowFlags.SetRendering(true);
	ShowFlags.SetStaticMeshes(true);
	ShowFlags.SetLandscape(true);
	ShowFlags.SetInstancedFoliage(true);
	ShowFlags.SetInstancedGrass(true);
	ShowFlags.SetInstancedStaticMeshes(true);
	ShowFlags.SetSkeletalMeshes(true);
}

void ARGBDCamera::ShowFlagsLit(FEngineShowFlags &ShowFlags) const
{
	ShowFlagsBasicSetting(ShowFlags);
	ShowFlags = FEngineShowFlags(EShowFlagInitMode::ESFIM_Game);
	ApplyViewMode(VMI_Lit, true, ShowFlags);
	ShowFlags.SetMaterials(true);
	ShowFlags.SetLighting(true);
	ShowFlags.SetPostProcessing(true);
	// ToneMapper needs to be enabled, otherwise the screen will be very dark
	ShowFlags.SetTonemapper(true);
	// TemporalAA needs to be disabled, otherwise the previous frame might contaminate current frame.
	// Check: https://answers.unrealengine.com/questions/436060/low-quality-screenshot-after-setting-the-actor-pos.html for detail
	ShowFlags.SetTemporalAA(false);
	ShowFlags.SetAntiAliasing(true);
	ShowFlags.SetEyeAdaptation(false); // Eye adaption is a slow temporal procedure, not useful for image capture
}

void ARGBDCamera::ShowFlagsPostProcess(FEngineShowFlags &ShowFlags) const
{
	ShowFlagsBasicSetting(ShowFlags);
	ShowFlags.SetPostProcessing(true);
	ShowFlags.SetPostProcessMaterial(true);

	GVertexColorViewMode = EVertexColorViewMode::Color;
}

void ARGBDCamera::ShowFlagsVertexColor(FEngineShowFlags &ShowFlags) const
{
	ShowFlagsLit(ShowFlags);
	ApplyViewMode(VMI_Lit, true, ShowFlags);

	// From MeshPaintEdMode.cpp:2942
	ShowFlags.SetMaterials(false);
	ShowFlags.SetLighting(false);
	ShowFlags.SetBSPTriangles(true);
	ShowFlags.SetVertexColors(true);
	ShowFlags.SetPostProcessing(false);
	ShowFlags.SetHMDDistortion(false);
	ShowFlags.SetTonemapper(false); // This won't take effect here

	GVertexColorViewMode = EVertexColorViewMode::Color;
}

void ARGBDCamera::ReadImage(UTextureRenderTarget2D *RenderTarget, TArray<FFloat16Color> &ImageData) const
{
	FTextureRenderTargetResource *RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	RenderTargetResource->ReadFloat16Pixels(ImageData);
}

void ARGBDCamera::ToColorImage(const TArray<FFloat16Color> &ImageData, uint8 *Bytes) const
{
	const FFloat16Color *itI = ImageData.GetData();
	uint8_t *itO = Bytes;

	// Converts Float colors to bytes
	for(size_t i = 0; i < ImageData.Num(); ++i, ++itI, ++itO)
	{
		*itO = (uint8_t)std::round((float)itI->B * 255.f);
		*++itO = (uint8_t)std::round((float)itI->G * 255.f);
		*++itO = (uint8_t)std::round((float)itI->R * 255.f);
	}
	return;
}

void ARGBDCamera::ToDepthImage(const TArray<FFloat16Color> &ImageData, uint8 *Bytes) const
{
	const FFloat16Color *itI = ImageData.GetData();
	uint16_t *itO = reinterpret_cast<uint16_t *>(Bytes);

	// Just copies the encoded Float16 values
	for(size_t i = 0; i < ImageData.Num(); ++i, ++itI, ++itO)
	{
		*itO = itI->R.Encoded;
	}
	return;
}

void ARGBDCamera::StoreImage(const uint8 *ImageData, const uint32 Size, const char *Name) const
{
	std::ofstream File(Name, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
	File.write(reinterpret_cast<const char *>(ImageData), Size);
	File.close();
	return;
}

/* Generates at least NumberOfColors different colors.
 * It takes MaxHue different Hue values and additional steps ind Value and Saturation to get
 * the number of needed colors.
 */
void ARGBDCamera::GenerateColors(const uint32_t NumberOfColors)
{
	const int32_t MaxHue = 50;
	// It shifts the next Hue value used, so that colors next to each other are not very similar. This is just important for humans
	const int32_t ShiftHue = 21;
	const float MinSat = 0.65;
	const float MinVal = 0.65;

	uint32_t HueCount = MaxHue;
	uint32_t SatCount = 1;
	uint32_t ValCount = 1;

	// Compute how many different Saturations and Values are needed
	int32_t left = std::max<int32_t>(0, NumberOfColors - HueCount);
	while(left > 0)
	{
		if(left > 0)
		{
			++ValCount;
			left = NumberOfColors - SatCount * ValCount * HueCount;
		}
		if(left > 0)
		{
			++SatCount;
			left = NumberOfColors - SatCount * ValCount * HueCount;
		}
	}

	const float StepHue = 360.0f / HueCount;
	const float StepSat = (1.0f - MinSat) / std::max(1.0f, SatCount - 1.0f);
	const float StepVal = (1.0f - MinVal) / std::max(1.0f, ValCount - 1.0f);

	ObjectColors.Reserve(SatCount * ValCount * HueCount);
	OUT_INFO(TEXT("Generating %d colors."), SatCount * ValCount * HueCount);

	FLinearColor HSVColor;
	for(uint32_t s = 0; s < SatCount; ++s)
	{
		HSVColor.G = 1.0f - s * StepSat;
		for(uint32_t v = 0; v < ValCount; ++v)
		{
			HSVColor.B = 1.0f - v * StepVal;
			for(uint32_t h = 0; h < HueCount; ++h)
			{
				HSVColor.R = ((h * ShiftHue) % MaxHue) * StepHue;
				ObjectColors.Add(HSVColor.HSVToLinearRGB().ToFColor(false));
				OUT_INFO(TEXT("Added color %d: %d %d %d"), ObjectColors.Num(), ObjectColors.Last().R, ObjectColors.Last().G, ObjectColors.Last().B);
			}
		}
	}
}

bool ARGBDCamera::ColorObject(AActor *Actor, const FString &name)
{
	const FColor &ObjectColor = ObjectColors[ObjectToColor[name]];
	TArray<UMeshComponent *> PaintableComponents;
	Actor->GetComponents<UMeshComponent>(PaintableComponents);

	for(auto MeshComponent : PaintableComponents)
	{
		if(MeshComponent == nullptr)
			continue;

		if(UStaticMeshComponent *StaticMeshComponent = Cast<UStaticMeshComponent>(MeshComponent))
		{
			if(UStaticMesh *StaticMesh = StaticMeshComponent->GetStaticMesh())
			{
				uint32 PaintingMeshLODIndex = 0;
				uint32 NumLODLevel = StaticMesh->RenderData->LODResources.Num();
				//check(NumLODLevel == 1);
				FStaticMeshLODResources &LODModel = StaticMesh->RenderData->LODResources[PaintingMeshLODIndex];
				FStaticMeshComponentLODInfo *InstanceMeshLODInfo = NULL;

				// PaintingMeshLODIndex + 1 is the minimum requirement, enlarge if not satisfied
				StaticMeshComponent->SetLODDataCount(PaintingMeshLODIndex + 1, StaticMeshComponent->LODData.Num());
				InstanceMeshLODInfo = &StaticMeshComponent->LODData[PaintingMeshLODIndex];

				{
					InstanceMeshLODInfo->OverrideVertexColors = new FColorVertexBuffer;

					FColor FillColor = FColor(255, 255, 255, 255);
					InstanceMeshLODInfo->OverrideVertexColors->InitFromSingleColor(FColor::White, LODModel.GetNumVertices());
				}

				uint32 NumVertices = LODModel.GetNumVertices();
				//check(InstanceMeshLODInfo->OverrideVertexColors);
				//check(NumVertices <= InstanceMeshLODInfo->OverrideVertexColors->GetNumVertices());

				for(uint32 ColorIndex = 0; ColorIndex < NumVertices; ++ColorIndex)
				{
					uint32 NumOverrideVertexColors = InstanceMeshLODInfo->OverrideVertexColors->GetNumVertices();
					uint32 NumPaintedVertices = InstanceMeshLODInfo->PaintedVertices.Num();
					InstanceMeshLODInfo->OverrideVertexColors->VertexColor(ColorIndex) = ObjectColor;
				}
				BeginInitResource(InstanceMeshLODInfo->OverrideVertexColors);
				StaticMeshComponent->MarkRenderStateDirty();
			}
		}
	}
	return true;
}

bool ARGBDCamera::ColorAllObjects()
{
	uint32_t NumberOfActors = 0;

	for(TActorIterator<AActor> ActItr(GetWorld()); ActItr; ++ActItr)
	{
		++NumberOfActors;
		FString ActorName = ActItr->GetName();
		OUT_INFO(TEXT("Actor with name: %s."), *ActorName);
	}
  
	OUT_INFO(TEXT("Found %d Actors."), NumberOfActors);
	GenerateColors(NumberOfActors * 2);

	for(TActorIterator<AActor> ActItr(GetWorld()); ActItr; ++ActItr)
	{
		FString ActorName = ActItr->GetName();
		if(!ObjectToColor.Contains(ActorName))
		{
			check(ColorsUsed < (uint32)ObjectColors.Num());
			ObjectToColor.Add(ActorName, ColorsUsed);
			OUT_INFO(TEXT("Adding color %d for object %s."), ColorsUsed, *ActorName);

			++ColorsUsed;
		}

		OUT_INFO(TEXT("Coloring object %s."), *ActorName);
		ColorObject(*ActItr, ActorName);
	}
	return true;
}

void ARGBDCamera::ProcessColor()
{
	while(true)
	{
		std::unique_lock<std::mutex> WaitLock(Priv->WaitColor);
		Priv->CVColor.wait(WaitLock, [this] {return Priv->DoColor; });
		Priv->DoColor = false;
		if(!this->Running) break;
		ToColorImage(ImageColor, Priv->Buffer->Color);

		Priv->DoneColor = true;
		Priv->CVDone.notify_one();
	}
}

void ARGBDCamera::ProcessDepth()
{
	while(true)
	{
		std::unique_lock<std::mutex> WaitLock(Priv->WaitDepth);
		Priv->CVDepth.wait(WaitLock, [this] {return Priv->DoDepth; });
		Priv->DoDepth = false;
		if(!this->Running) break;
		ToDepthImage(ImageDepth, Priv->Buffer->Depth);

		// Wait for both other processing threads to be done.
		std::unique_lock<std::mutex> WaitDoneLock(Priv->WaitDone);
		Priv->CVDone.wait(WaitDoneLock, [this] {return Priv->DoneColor && Priv->DoneObject; });

		Priv->DoneColor = false;
		Priv->DoneObject = false;

		// Complete Buffer
		Priv->Buffer->DoneWriting();
	}
}

void ARGBDCamera::ProcessObject()
{
	while(true)
	{
		std::unique_lock<std::mutex> WaitLock(Priv->WaitObject);
		Priv->CVObject.wait(WaitLock, [this] {return Priv->DoObject; });
		Priv->DoObject = false;
		if(!this->Running) break;
		ToColorImage(ImageObject, Priv->Buffer->Object);

		Priv->DoneObject = true;
		Priv->CVDone.notify_one();
	}
}
