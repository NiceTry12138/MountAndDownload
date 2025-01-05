// Fill out your copyright notice in the Description page of Project Settings.


#include "MountPakPlayerController.h"
#include "IPlatformFilePak.h"
#include "HAL/PlatformFileManager.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetSystemLibrary.h"

void AMountPakPlayerController::BeginPlay()
{
	Super::BeginPlay();

}

void AMountPakPlayerController::MountPakWithPath(const FString& InPath)
{
	PakPlatformFile = (FPakPlatformFile*)FPlatformFileManager::Get().FindPlatformFile(TEXT("PakFile"));
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

bool AMountPakPlayerController::MountPakTest(const FString& InPath, int InPakOrder)
{
	const FString PakFileName = TEXT("PakFile");
	FPakPlatformFile* NewPakPlatformFile = static_cast<FPakPlatformFile*>(FPlatformFileManager::Get().FindPlatformFile(*PakFileName));
	if (!NewPakPlatformFile)
	{
		// Create a pak platform file and mount the feature pack file.
		NewPakPlatformFile = static_cast<FPakPlatformFile*>(FPlatformFileManager::Get().GetPlatformFile(*PakFileName));
		NewPakPlatformFile->Initialize(&FPlatformFileManager::Get().GetPlatformFile(), TEXT(""));
	}


	TRefCountPtr<FPakFile> PakFilePtr = new FPakFile(NewPakPlatformFile, *InPath, false);
	FString PakMountPoint = PakFilePtr->GetMountPoint();

	bool bMounted = false;


	if (NewPakPlatformFile->Mount(*InPath, InPakOrder))
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("MountPoint ==> ") + PakMountPoint);
		
		TArray<FString> FileNames;
		PakFilePtr->GetPrunedFilenames(FileNames);
		for (const auto& ItemName : FileNames)
		{
			UKismetSystemLibrary::PrintString(GetWorld(), TEXT("\tItemName ==> ") + ItemName);
		}
	}

	return true;
}

bool AMountPakPlayerController::SpawnMountFile(bool bTest)
{
	static const FString DefaultMesh = TEXT("/Game/StarterContent/Architecture/Wall_Window_400x300");
	UStaticMesh* DefaultSM = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *DefaultMesh));
	 
	auto MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), GetPawn()->GetActorTransform());
	MeshActor->SetMobility(EComponentMobility::Movable);
	if (IsValid(DefaultSM))
	{
		MeshActor->GetStaticMeshComponent()->SetStaticMesh(DefaultSM);
	}

	FString MaterialPath = TEXT("/Game/Mount/M_MountTest");
	FString WallPath = TEXT("/Game/Mount/Wall");

	if (bTest)
	{
		MaterialPath = TEXT("/Game/Mount/Mesh/M_MountTest");
		WallPath = TEXT("/Game/Mount/Mesh/Wall");
	}

	UMaterial* Material = LoadObject<UMaterial>(nullptr, *MaterialPath);

	if (IsValid(Material))
	{
		MeshActor->GetStaticMeshComponent()->SetMaterial(0, Material);
	}

	UStaticMesh* SM = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *WallPath));

	if (IsValid(SM))
	{
		MeshActor->GetStaticMeshComponent()->SetStaticMesh(SM);
	}

	return false;
}
