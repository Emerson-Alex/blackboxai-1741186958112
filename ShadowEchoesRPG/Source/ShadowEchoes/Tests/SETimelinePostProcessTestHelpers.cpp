#include "SETimelinePostProcessTestHelpers.h"
#include "SETimelineTestLogger.h"
#include "SETimelineTestMetrics.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Engine/Texture2D.h"
#include "Engine/Light.h"
#include "HighResScreenshot.h"
#include "ImageUtils.h"

bool FSETimelinePostProcessTestHelpers::SetupTestEnvironment(UWorld* World, const SETimelineTest::FTestConfig& Config)
{
    if (!World)
    {
        HandleTestError(TEXT("Invalid world"));
        return false;
    }

    InitializeTestEnvironment();
    SetupTestScene(World);
    SetupTestViewport();
    SetupTestRendering();

    ValidateTestEnvironment();
    return true;
}

AActor* FSETimelinePostProcessTestHelpers::CreateTestActor(UWorld* World, const FString& Name)
{
    if (!World)
    {
        HandleTestError(TEXT("Invalid world"));
        return nullptr;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = *Name;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* Actor = World->SpawnActor<AActor>(SpawnParams);
    if (!Actor)
    {
        HandleTestError(TEXT("Failed to spawn actor"));
        return nullptr;
    }

    return Actor;
}

USETimelinePostProcessComponent* FSETimelinePostProcessTestHelpers::CreatePostProcessComponent(AActor* Actor)
{
    if (!Actor)
    {
        HandleTestError(TEXT("Invalid actor"));
        return nullptr;
    }

    USETimelinePostProcessComponent* Component = NewObject<USETimelinePostProcessComponent>(Actor);
    if (!Component)
    {
        HandleTestError(TEXT("Failed to create post-process component"));
        return nullptr;
    }

    Component->RegisterComponent();
    return Component;
}

void FSETimelinePostProcessTestHelpers::SetupPostProcessParameters(USETimelinePostProcessComponent* Component, const FTimelinePostProcessParameters& Parameters)
{
    if (!Component)
    {
        HandleTestError(TEXT("Invalid component"));
        return;
    }

    Component->SetParameters(Parameters);
}

bool FSETimelinePostProcessTestHelpers::ValidatePostProcessEffect(USETimelinePostProcessComponent* Component, const FTimelinePostProcessParameters& ExpectedParams)
{
    if (!Component)
    {
        HandleTestError(TEXT("Invalid component"));
        return false;
    }

    const FTimelinePostProcessParameters& ActualParams = Component->GetParameters();

    bool bValid = true;
    bValid &= FMath::IsNearlyEqual(ActualParams.Intensity, ExpectedParams.Intensity);
    bValid &= FMath::IsNearlyEqual(ActualParams.Duration, ExpectedParams.Duration);
    bValid &= ActualParams.BlendMode == ExpectedParams.BlendMode;

    if (!bValid)
    {
        HandleTestError(TEXT("Post-process parameters do not match expected values"));
    }

    return bValid;
}

bool FSETimelinePostProcessTestHelpers::CaptureScreenshot(const FString& TestName, const FString& OutputPath)
{
    const FString ScreenshotPath = FPaths::Combine(OutputPath, FString::Printf(TEXT("%s.png"), *TestName));
    
    FHighResScreenshotConfig& HighResScreenshotConfig = GetHighResScreenshotConfig();
    HighResScreenshotConfig.FilenameOverride = ScreenshotPath;
    HighResScreenshotConfig.SetResolution(1920, 1080);
    HighResScreenshotConfig.bMaskEnabled = false;
    HighResScreenshotConfig.CaptureRegion = FIntRect(0, 0, 1920, 1080);

    return FScreenshotRequest::RequestScreenshot(ScreenshotPath, false, false);
}

float FSETimelinePostProcessTestHelpers::CompareScreenshots(const FString& ScreenshotA, const FString& ScreenshotB)
{
    TArray<FColor> ImageA;
    TArray<FColor> ImageB;
    FIntPoint SizeA;
    FIntPoint SizeB;

    if (!FImageUtils::LoadImage(*ScreenshotA, ImageA, SizeA) ||
        !FImageUtils::LoadImage(*ScreenshotB, ImageB, SizeB))
    {
        HandleTestError(TEXT("Failed to load screenshots"));
        return 1.0f;
    }

    if (SizeA != SizeB || ImageA.Num() != ImageB.Num())
    {
        HandleTestError(TEXT("Screenshot dimensions do not match"));
        return 1.0f;
    }

    float TotalDifference = 0.0f;
    const int32 NumPixels = ImageA.Num();

    for (int32 i = 0; i < NumPixels; ++i)
    {
        const FColor& ColorA = ImageA[i];
        const FColor& ColorB = ImageB[i];

        const float DiffR = FMath::Abs(ColorA.R - ColorB.R) / 255.0f;
        const float DiffG = FMath::Abs(ColorA.G - ColorB.G) / 255.0f;
        const float DiffB = FMath::Abs(ColorA.B - ColorB.B) / 255.0f;
        const float DiffA = FMath::Abs(ColorA.A - ColorB.A) / 255.0f;

        TotalDifference += (DiffR + DiffG + DiffB + DiffA) / 4.0f;
    }

    return TotalDifference / NumPixels;
}

UWorld* FSETimelinePostProcessTestHelpers::GetTestWorld()
{
    if (GEngine)
    {
        if (FWorldContext* WorldContext = GEngine->GetWorldContextFromPIEInstance(0))
        {
            return WorldContext->World();
        }
    }
    return nullptr;
}

void FSETimelinePostProcessTestHelpers::SetupTestScene(UWorld* World)
{
    if (!World)
    {
        HandleTestError(TEXT("Invalid world"));
        return;
    }

    // Create test actor with mesh and material
    AActor* TestActor = CreateTestActor(World, TEXT("TestActor"));
    if (TestActor)
    {
        UStaticMeshComponent* MeshComponent = CreateTestMesh(TestActor);
        if (MeshComponent)
        {
            UMaterialInterface* Material = CreateTestMaterial();
            if (Material)
            {
                MeshComponent->SetMaterial(0, Material);
            }
        }
    }

    // Create light
    ALight* Light = World->SpawnActor<ALight>();
    if (Light)
    {
        Light->SetActorLocation(FVector(200, 200, 200));
    }

    // Create camera
    ACameraActor* Camera = World->SpawnActor<ACameraActor>();
    if (Camera)
    {
        Camera->SetActorLocation(FVector(0, 0, 200));
        Camera->SetActorRotation(FRotator(-30, 0, 0));
    }
}

UStaticMeshComponent* FSETimelinePostProcessTestHelpers::CreateTestMesh(AActor* Actor)
{
    if (!Actor)
    {
        return nullptr;
    }

    UStaticMeshComponent* MeshComponent = NewObject<UStaticMeshComponent>(Actor);
    if (MeshComponent)
    {
        MeshComponent->RegisterComponent();
        Actor->AddInstanceComponent(MeshComponent);
        
        // Set default cube mesh
        if (UStaticMesh* CubeMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Engine/BasicShapes/Cube"))))
        {
            MeshComponent->SetStaticMesh(CubeMesh);
        }
    }

    return MeshComponent;
}

UMaterialInterface* FSETimelinePostProcessTestHelpers::CreateTestMaterial()
{
    // Load default material
    return Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial")));
}

void FSETimelinePostProcessTestHelpers::InitializeTestEnvironment()
{
    LogTestMessage(TEXT("Initializing test environment"));
}

void FSETimelinePostProcessTestHelpers::ValidateTestEnvironment()
{
    LogTestMessage(TEXT("Validating test environment"));
}

void FSETimelinePostProcessTestHelpers::SetupTestViewport()
{
    LogTestMessage(TEXT("Setting up test viewport"));
}

void FSETimelinePostProcessTestHelpers::SetupTestRendering()
{
    LogTestMessage(TEXT("Setting up test rendering"));
}

void FSETimelinePostProcessTestHelpers::HandleTestError(const FString& ErrorMessage)
{
    FSETimelineTestLogger::Get().LogError(TEXT("TestHelpers"), ErrorMessage);
}

void FSETimelinePostProcessTestHelpers::LogTestMessage(const FString& Message)
{
    FSETimelineTestLogger::Get().LogInfo(TEXT("TestHelpers"), Message);
}
