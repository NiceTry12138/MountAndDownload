// Copyright Epic Games, Inc. All Rights Reserved.

#include "MountPakBPLibrary.h"
#include "MountPak.h"

UMountPakBPLibrary::UMountPakBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

float UMountPakBPLibrary::MountPakSampleFunction(float Param)
{
	return -1;
}

void UMountPakBPLibrary::GeneratePakWithInfos(const TArray<FGeneratePakInfo>& InInfos)
{
	for (const auto& InfoItem : InInfos)
	{
		GeneratePakWithOneInfo(InfoItem);
	}
}

void UMountPakBPLibrary::GeneratePakWithOneInfo(const FGeneratePakInfo& InInfos)
{
	static FString EngineDir = FPaths::EngineDir();
	static const FString RelativeUnrealPakExe = TEXT("Binaries/Win64/UnrealPak.exe");
	static FString UnrealPakExe = FPaths::ConvertRelativePathToFull(EngineDir + RelativeUnrealPakExe);

	FString Compress;
	if (InInfos.bIsCompress)
	{
		Compress = TEXT("-compress");
	}
	FString GeneratePakFileContent;
	for (const FString& Item : InInfos.AssetFolds)
	{
		if (FPaths::FileExists(Item) || FPaths::DirectoryExists(Item))
		{
			GeneratePakFileContent += TEXT("\"") + UpdatePathEnd(Item) + TEXT("\" \"") + GetMountPoint(Item) + TEXT("\"") + Compress + TEXT("\r\n");
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Input File Path Doesn't Exists => %s"), *Item);
		}
	}

	static FString ProjectPath = FPaths::ProjectDir();
	static FString GenTxtPath = TEXT("Saved/MountFile/GenFile.txt");
	static FString GeneratePath = ProjectPath + TEXT("/") + GenTxtPath;
	static FString GenerateTxtFullPath =  FPaths::ConvertRelativePathToFull(GeneratePath);

	FFileHelper::SaveStringToFile(GeneratePakFileContent, *GenerateTxtFullPath);
	
	FString PakFileName = InInfos.OutputPakFileName;
	if (!PakFileName.EndsWith(TEXT(".pak")))
	{
		PakFileName += TEXT(".pak");
	}
	FString GeneratePakPath = FPaths::Combine(InInfos.OutputFolder, PakFileName);
	static FString GeneratePakFullPath = FPaths::ConvertRelativePathToFull(GeneratePakPath);

	UE_LOG(LogTemp, Log, TEXT("UnrealPakExe Path %s, Output Txt Path %s, Output Pak Path %s"), *UnrealPakExe, *GenerateTxtFullPath, *GeneratePakFullPath);

#if PLATFORM_WINDOWS
	FString URL = TEXT("cmd.exe");
	FString Params = FString::Printf(TEXT("/c \"%s\" %s -create=%s\""), *UnrealPakExe, *GeneratePakFullPath, *GenerateTxtFullPath);
	UE_LOG(LogTemp, Log, TEXT("Run param => %s"), *Params);
	// if (FPlatformProcess::CreateProc(*URL, *Params, true, false, false, NULL, 0, NULL, NULL).IsValid())
	// {
	// 	UE_LOG(LogTemp, Log, TEXT("Run Success "));
	// }
#endif
}

void UMountPakBPLibrary::MountPakWithPath(const FString& InPath)
{
	
}

FString UMountPakBPLibrary::GetMountPoint(const FString& InPath)
{
	FString Left, Right;
	InPath.Split(TEXT("Content"), &Left, &Right);

	FString Result = FPaths::ProjectDir() + TEXT("Content") + Right;

	//return UpdatePathEnd(Result);
	return Result;
}

FString UMountPakBPLibrary::UpdatePathEnd(const FString& InPath)
{
	static const FString AssetFileExtend = TEXT(".uasset");
	static const FString UexpFileExtend = TEXT(".uasset");

	if (!InPath.EndsWith(AssetFileExtend) && !InPath.EndsWith(UexpFileExtend))
	{
		return InPath + TEXT("/*");
	}
	return InPath;
}

