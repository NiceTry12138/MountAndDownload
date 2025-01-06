// Fill out your copyright notice in the Description page of Project Settings.


#include "Tool/GenerateToolLibrary.h"

FString GenerateToolLibrary::GeneratePakConfigTxt(const FString& PakFileName, const TArray<FString>& InputDirects, bool bCompress)
{
	TSet<FString> TotalConfig;
	for (const FString& InputDirect : InputDirects)
	{
		FString FullInputDirectPath = FPaths::ConvertRelativePathToFull(InputDirect);
		TArray<FString> AllFileInDirect;
		IFileManager::Get().FindFilesRecursive(AllFileInDirect, *FullInputDirectPath, TEXT("*"), true, false);

		// F:/3_UEDemo/MountTest/Saved/Cooked/Windows/MountTest/Content/Characters/Mannequins/Animations
		// ../../../MountTest/Content/Characters/Mannequins/Animations
		for (const auto& FilePath : AllFileInDirect)
		{
			FString LStr, RStr;
			FilePath.Split(TEXT("Content"), &LStr, &RStr);
			FString RelativePath = FPaths::Combine(TEXT("../../../"), FApp::GetProjectName(), TEXT("Content"), RStr);
			FString Compress = bCompress ? TEXT("-compress") : TEXT("");
			FString ItemConfig = TEXT("\"") + FilePath + TEXT("\" \"") + RelativePath + TEXT("\" ") + Compress;
			TotalConfig.Add(ItemConfig);
			UE_LOG(LogTemp, Log, TEXT("Item File Config -> %s"), *ItemConfig);
		}
	}

	FString SaveFileContent;

	for (const auto& Config : TotalConfig)
	{
		SaveFileContent += Config + TEXT("\r\n");
	}

	FString BaseFileName = FPaths::GetBaseFilename(PakFileName) + TEXT(".txt");
	FString SavePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("MountPak"), TEXT("__CacheTxt"), BaseFileName);

	FFileHelper::SaveStringToFile(SaveFileContent, *SavePath);
	return SavePath;
}
