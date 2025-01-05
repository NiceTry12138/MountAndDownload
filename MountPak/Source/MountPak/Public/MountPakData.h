// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MountPakData.generated.h"

USTRUCT(BlueprintType)
struct FGeneratePakInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> AssetFolds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCompress = true;;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OutputFolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OutputPakFileName;
};