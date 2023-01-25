// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MillicastCameraActor.h"
#include <Engine/TextureRenderTarget2D.h>

#include "MillicastCameraPawn.generated.h"

UCLASS()
class MILLICASTPUBLISHDEMO_API AMillicastCameraPawn : public APawn
{
	GENERATED_BODY()

	FVector2D MovementInput;
	FVector2D CameraInput;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, AssetRegistrySearchable, 
		META = (DisplayName = "MillicastCameraPawn", AllowPrivateAccess = true))
	AMillicastCameraActor* CameraComp;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComp;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Render Target", META = (DisplayName = "Render Target"))
	UTextureRenderTarget2D* RenderTarget = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Render Target", META = (DisplayName = "Logo"))
	UTexture2D* LogoRenderTarget = nullptr;

public:

	// Sets default values for this pawn's properties
	AMillicastCameraPawn();

	void CopyTexture();

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void PitchCamera(float AxisValue);
	void YawCamera(float AxisValue);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
