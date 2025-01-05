// Copyright Epic Games, Inc. All Rights Reserved.

#include "DownloadData.h"

UHttpDownloadManagerSettings::UHttpDownloadManagerSettings()
{
	CacheDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("HttpFiles"));
}

UHttpDownloadManagerSettings* UHttpDownloadManagerSettings::Get()
{
	return GetMutableDefault<UHttpDownloadManagerSettings>();
}
