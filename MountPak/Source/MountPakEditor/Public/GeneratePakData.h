// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GeneratePakData.generated.h"

USTRUCT(BlueprintType)
struct FGeneratePakItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SaveFileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SaveDirectory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString InDirectory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCompress = true;

	int32 ItemID = -1;
};
