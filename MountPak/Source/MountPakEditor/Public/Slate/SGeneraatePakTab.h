// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Docking/SDockTab.h"
#include "GeneratePakData.h"

class SGeneratePak;
class SOverlay;
class FMonitoredProcess;

/**
 * 
 */
class MOUNTPAKEDITOR_API SGeneratePakTab : public SDockTab
{
public:
	SLATE_BEGIN_ARGS(SGeneratePakTab)
	{
	}
	SLATE_END_ARGS()

	virtual void Construct(const FArguments& InArgs);
	virtual ~SGeneratePakTab();

protected:
	void InitData();
	void InitWidget();
	
	void StartGenerate();
	void FinishGenerate();

protected:
	void OnGenerate();
	
	void HandleGenerateProcessCanceled();
	void HandleGenerateProcessCompleted(int32 ReturnCode);
	static void HandleGenerateProcessOutput(FString Output);

	void Tick(float DeltaTime);
private:
	TSharedPtr<SGeneratePak> MainGeneratePak;
	TSharedPtr<SOverlay> MainOverlay;

private:
	FString UnrealPakPath;
	bool bIsRunning = false;

	TSharedPtr<FMonitoredProcess> GeneratePakProcess;		// UnrealPak 进程
	TArray<FGeneratePakItemData> WaitGeneratePakDatas;		// 数据源
	TArray<int> GeneratedIndex;								// 已经处理过的 Index 集合
	int32 WaitGenerateIndex = 0;							// 待处理的序号集合
};
