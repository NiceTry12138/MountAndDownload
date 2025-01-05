// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MountPakData.h"
#include "MountPakBPLibrary.generated.h"

UCLASS()
class UMountPakBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Sample function", Keywords = "MountPak sample test testing"), Category = "MountPakTesting")
	static float MountPakSampleFunction(float Param);

	UFUNCTION(BlueprintCallable)
	static void GeneratePakWithInfos(const TArray<FGeneratePakInfo>& InInfos);

	UFUNCTION(BlueprintCallable)
	static void GeneratePakWithOneInfo(const FGeneratePakInfo& InInfos);

	UFUNCTION(BlueprintCallable)
	static void MountPakWithPath(const FString& InPath);

private:

	static FString GetMountPoint(const FString& InPath);
	static FString UpdatePathEnd(const FString& InPath);
};
