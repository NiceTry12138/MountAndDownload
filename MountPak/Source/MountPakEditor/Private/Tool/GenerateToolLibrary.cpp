// Fill out your copyright notice in the Description page of Project Settings.


#include "Tool/GenerateToolLibrary.h"

FString GenerateToolLibrary::GeneratePakConfigTxt(const FString& PakFileName, const FString& InputDirect, bool bCompress)
{
	FString FullInputDirectPath = FPaths::ConvertRelativePathToFull(InputDirect);
	TArray<FString> AllFileInDirect;
	IFileManager::Get().FindFilesRecursive(AllFileInDirect, *FullInputDirectPath, TEXT("*"), true, false);

	FString TotalConfig;
	// F:/3_UEDemo/MountTest/Saved/Cooked/Windows/MountTest/Content/Characters/Mannequins/Animations
	// ../../../MountTest/Content/Characters/Mannequins/Animations
	for (const auto& FilePath : AllFileInDirect)
	{
		FString LStr, RStr;
		FilePath.Split(TEXT("Content"), &LStr, &RStr);
		FString RelativePath = FPaths::Combine(TEXT("../../../"), FApp::GetProjectName(), TEXT("Content"), RStr);
		FString Compress = bCompress ? TEXT("-compress") : TEXT("");
		FString ItemConfig = TEXT("\"") + FilePath + TEXT("\" \"") + RelativePath + TEXT("\" ") + Compress;
		TotalConfig += ItemConfig + TEXT("\r\n");
		UE_LOG(LogTemp, Log, TEXT("Item File Config -> %s"), *ItemConfig);
	}

	FString BaseFileName = FPaths::GetBaseFilename(PakFileName) + TEXT(".txt");
	FString SavePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("MountPak"), TEXT("__CacheTxt"), BaseFileName);

	FFileHelper::SaveStringToFile(TotalConfig, *SavePath);
	return SavePath;
}
