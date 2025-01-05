// Fill out your copyright notice in the Description page of Project Settings.


#include "MountPakActor.h"

#include "IPlatformFilePak.h"
#include "HAL/PlatformFileManager.h"

// Sets default values
AMountPakActor::AMountPakActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// OldPlatform = &FPlatformFileManager::Get().GetPlatformFile();
	// FPakPlatformFile();
	
	PakPlatformFile = (FPakPlatformFile*)FPlatformFileManager::Get().FindPlatformFile(TEXT("PakFile"));
}

void AMountPakActor::MountPakWithPath(const FString& InPath)
{
	if (PakPlatformFile == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Can't Create PakPlatformFile"));
		return;
	}

	TRefCountPtr<FPakFile> PakFilePtr = new FPakFile(PakPlatformFile, *InPath, false);
	FString PakMountPoint = PakFilePtr->GetMountPoint();

	UE_LOG(LogTemp, Log, TEXT("Pak InPath = %s, MountPoint = %s"), *InPath, *PakMountPoint);

	if (PakPlatformFile->Mount(*InPath, 1, *PakMountPoint))
	{
		TArray<FString> FountFileNames;
		PakFilePtr->FindPrunedFilesAtPath(FountFileNames, *PakMountPoint, true, false, false);

		for (const FString& FileName : FountFileNames)
		{
			UE_LOG(LogTemp, Log, TEXT("In Pak FileName = %s"), *FileName);
			if (FileName.EndsWith(TEXT(".uasset")))
			{
				FString NewFileName = FileName;
				NewFileName.RemoveFromEnd(TEXT(".uasset"));
				int32 Pos = NewFileName.Find(TEXT("/Content/"));
			}
		}
	}
}

// Called when the game starts or when spawned
void AMountPakActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMountPakActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

