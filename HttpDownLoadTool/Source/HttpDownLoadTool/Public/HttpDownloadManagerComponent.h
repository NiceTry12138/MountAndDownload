// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AsyncDownadFile.h"
#include "HttpDownloadManagerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UHttpDownloadManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:	
	UHttpDownloadManagerSubsystem();
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	void AddHttpDownloadFile(UAsyncDownloadFile* HttpFile);
	void NotifyFaild(const UAsyncDownloadFile* Querier);
	void NotifySucceeded(const UAsyncDownloadFile* Querier);

protected:
	void ExecuteDownloadFile();

private:
	UPROPERTY()
	TArray<UAsyncDownloadFile*> DownloadHttpFiles;
};
