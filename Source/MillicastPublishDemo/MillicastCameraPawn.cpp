// Fill out your copyright notice in the Description page of Project Settings.

#include "MillicastCameraPawn.h"
#include "GlobalShader.h"
#include "CommonRenderResources.h"
#include "ScreenRendering.h"

// #include "MillicastViewportComponent.h"

// Sets default values
AMillicastCameraPawn::AMillicastCameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	StaticMeshComp = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	CameraComp = CreateDefaultSubobject<AMillicastCameraActor>(TEXT("Millicast Camera Actor"));

	StaticMeshComp->SetupAttachment(RootComponent);

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	FCoreDelegates::OnBeginFrameRT.AddLambda([this]() {
		CopyTexture();
		});
}

void AMillicastCameraPawn::CopyTexture()
{
	if (!RenderTarget || !LogoRenderTarget)
	{
		return;
	}

	IRendererModule* RendererModule = &FModuleManager::GetModuleChecked<IRendererModule>("Renderer");

	auto DestTexture = RenderTarget->GetResource()->GetTexture2DRHI();
	auto SourceTexture = LogoRenderTarget->GetResource()->GetTexture2DRHI();

	FRHICommandListImmediate& RHICmdList = FRHICommandListExecutor::GetImmediateCommandList();
	RHICmdList.Transition(FRHITransitionInfo(SourceTexture, ERHIAccess::Unknown, ERHIAccess::SRVMask));
	RHICmdList.Transition(FRHITransitionInfo(DestTexture, ERHIAccess::Unknown, ERHIAccess::RTV));
	
	FRHIRenderPassInfo RPInfo(DestTexture, ERenderTargetActions::Load_Store);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("CopyTexture"));
	{
		FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
		TShaderMapRef<FScreenVS> VertexShader(ShaderMap);
		TShaderMapRef<FScreenPS> PixelShader(ShaderMap);

		RHICmdList.SetViewport(0, 0, 0.0f, SourceTexture->GetSizeX(), SourceTexture->GetSizeY(), 1.0f);

		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
		GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
		GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
		GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
		GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
		GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
		GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
		GraphicsPSOInit.PrimitiveType = PT_TriangleList;
		SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);

		PixelShader->SetParameters(RHICmdList, TStaticSamplerState<SF_Bilinear>::GetRHI(), SourceTexture);
		
		FIntPoint TargetBufferSize(DestTexture->GetSizeX(), DestTexture->GetSizeY());
		RendererModule->DrawRectangle(RHICmdList, 0, 0, // Dest X, Y
			DestTexture->GetSizeX(),					// Dest Width
			DestTexture->GetSizeY(),					// Dest Height
			0, 0,										// Source U, V
			1, 1,										// Source USize, VSize
			TargetBufferSize,							// Target buffer size
			FIntPoint(1, 1),							// Source texture size
			VertexShader, EDRF_Default);
	}

	RHICmdList.EndRenderPass();

	RHICmdList.Transition(FRHITransitionInfo(SourceTexture, ERHIAccess::SRVMask, ERHIAccess::CopySrc));
	RHICmdList.Transition(FRHITransitionInfo(DestTexture, ERHIAccess::RTV, ERHIAccess::CopyDest));
}

void AMillicastCameraPawn::MoveForward(float AxisValue)
{
	MovementInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void AMillicastCameraPawn::MoveRight(float AxisValue)
{
	MovementInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void AMillicastCameraPawn::PitchCamera(float AxisValue)
{
	CameraInput.X = AxisValue;
}

void AMillicastCameraPawn::YawCamera(float AxisValue)
{
	CameraInput.Y = AxisValue;
}

// Called when the game starts or when spawned
void AMillicastCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMillicastCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += CameraInput.X;
	NewRotation.Pitch += CameraInput.Y;
	SetActorRotation(NewRotation);
	CameraComp->SetActorRotation(NewRotation);

	if (!MovementInput.IsZero())
	{
		MovementInput = MovementInput.GetSafeNormal() * 100.f;
		FVector NewLocation = GetActorLocation();
		NewLocation += GetActorForwardVector() * MovementInput.X * DeltaTime * 10;
		NewLocation += GetActorRightVector() * MovementInput.Y * DeltaTime;
		SetActorLocation(NewLocation);
		CameraComp->SetActorLocation(NewLocation);
	}
}

// Called to bind functionality to input
void AMillicastCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveCameraForward", this, &AMillicastCameraPawn::MoveForward);
	InputComponent->BindAxis("MoveCameraRight", this, &AMillicastCameraPawn::MoveRight);
	InputComponent->BindAxis("CameraPitch", this, &AMillicastCameraPawn::PitchCamera);
	InputComponent->BindAxis("CameraYaw", this, &AMillicastCameraPawn::YawCamera);
}

