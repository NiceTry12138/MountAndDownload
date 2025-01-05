// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MountPakActor.generated.h"

class IPlatformFile;
class FPakPlatformFile;

UCLASS()
class MOUNTPAK_API AMountPakActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMountPakActor();

	UFUNCTION(BlueprintCallable)
	void MountPakWithPath(const FString& InPath);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FPakPlatformFile* PakPlatformFile = nullptr;
};
