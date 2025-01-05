// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpDownloadManagerComponent.h"

UHttpDownloadManagerSubsystem::UHttpDownloadManagerSubsystem()
{
}

bool UHttpDownloadManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UHttpDownloadManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UHttpDownloadManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UHttpDownloadManagerSubsystem::AddHttpDownloadFile(UAsyncDownloadFile* HttpFile)
{
	if (!IsValid(HttpFile))
	{
		return;
	}

	DownloadHttpFiles.AddUnique(HttpFile);

	ExecuteDownloadFile();
}

void UHttpDownloadManagerSubsystem::NotifyFaild(const UAsyncDownloadFile* Querier)
{
	ExecuteDownloadFile();
}

void UHttpDownloadManagerSubsystem::NotifySucceeded(const UAsyncDownloadFile* Querier)
{
	ExecuteDownloadFile();
}

void UHttpDownloadManagerSubsystem::ExecuteDownloadFile()
{
	// TODO 从 Settings 中获取
	int32 MaxTryCount = UHttpDownloadManagerSettings::Get()->MaxTryCount;
	int32 MaxParallel = UHttpDownloadManagerSettings::Get()->MaxParallel;
	
	int32 NumActivateTask = 0;
	TArray<UAsyncDownloadFile*> ProbTasks;

	for (int32 Index = DownloadHttpFiles.Num() - 1; Index >= 0; --Index)
	{
		auto HttpFile = DownloadHttpFiles[Index];
		if (!IsValid(HttpFile))
		{
			continue;
		}
		
		switch (HttpFile->GetState())
		{
		case EHttpDownloadState::Downloading:
			++NumActivateTask;
			break;
		case EHttpDownloadState::Failed:
			DownloadHttpFiles.RemoveAt(Index);
			break;
		case EHttpDownloadState::Ready:
			ProbTasks.AddUnique(HttpFile);
			break;
		case EHttpDownloadState::Retry:
			{
				if (HttpFile->GetTryCount() <= MaxTryCount)
				{
					ProbTasks.AddUnique(HttpFile);
				}
			}
			break;
		case EHttpDownloadState::Success:
			DownloadHttpFiles.RemoveAt(Index);
			break;
		}
	}

	int CouldAddNum = FMath::Max(0, MaxParallel - NumActivateTask);
	for (int Index = 0; Index < CouldAddNum && Index < ProbTasks.Num(); ++Index)
	{
		auto HttpFile = ProbTasks[Index];
		HttpFile->StartDownload();
	}
}
