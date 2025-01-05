// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MountPakPlayerController.generated.h"

class FPakPlatformFile;

/**
 * 
 */
UCLASS()
class MOUNTPAK_API AMountPakPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Exec)
	void MountPakWithPath(const FString& InPath);

	UFUNCTION(Exec)
	bool MountPakTest(const FString& InPath, int InPakOrder);

	UFUNCTION(BlueprintCallable, Exec)
	bool SpawnMountFile(bool bTest = false);

private:
	FPakPlatformFile* PakPlatformFile = nullptr;
};
