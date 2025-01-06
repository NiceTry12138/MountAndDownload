// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/SGeneraatePakTab.h"
#include "Slate/SGeneratePak.h"
#include "Tool/GenerateToolLibrary.h"
#include "Widgets/SOverlay.h"
#include "Misc/MonitoredProcess.h"

void SGeneratePakTab::Construct(const FArguments& InArgs)
{
	InitData();

	InitWidget();

	FSlateApplication::Get().OnPreTick().AddSP(this, &SGeneratePakTab::Tick);
}

SGeneratePakTab::~SGeneratePakTab()
{
	FinishGenerate();
	FSlateApplication::Get().OnPreTick().RemoveAll(this);
}

void SGeneratePakTab::Tick(float DeltaTime)
{
	// 当前进程正在运行 则不做处理 等到进程结束再说
	// 当前没有运行 则不做处理
	if (GeneratePakProcess.IsValid() || !bIsRunning)
	{
		return;
	}

	// 当前没有要跑的了 但是还在运行 要执行 Finish 清除流程
	if (WaitGenerateIndex >= WaitGeneratePakDatas.Num() && bIsRunning)
	{
		FinishGenerate();
		return;
	}

	// 当前还有要跑的 继续跑
	if (WaitGenerateIndex < WaitGeneratePakDatas.Num())
	{
		StartGenerate();
		return;
	}
}

void SGeneratePakTab::InitData()
{
	UnrealPakPath = FPaths::Combine(FPaths::EngineDir(), TEXT("Binaries"), TEXT("Win64"), TEXT("UnrealPak.exe"));
}

void SGeneratePakTab::InitWidget()
{
	MainGeneratePak = SNew(SGeneratePak)
						.OnGenerateAllPak(this, &SGeneratePakTab::OnGenerate);

	MainOverlay = SNew(SOverlay);
	MainOverlay->AddSlot()
		[
			MainGeneratePak.ToSharedRef()
		];
	
	SDockTab::Construct(SDockTab::FArguments().TabRole(NomadTab)
		[
			MainOverlay.ToSharedRef()
		]
	);
}

void SGeneratePakTab::StartGenerate()
{
	auto Data = WaitGeneratePakDatas[WaitGenerateIndex];
	FString TxtPath = GenerateToolLibrary::GeneratePakConfigTxt(Data.SaveFileName, Data.InDirectory, Data.bIsCompress);
	FString SavePath = FPaths::Combine(Data.SaveDirectory, Data.SaveFileName);

	FString Params = SavePath + TEXT(" -create=") + TxtPath;
	
	GeneratePakProcess = MakeShared<FMonitoredProcess>(UnrealPakPath, Params, true, true);
	GeneratePakProcess->OnCanceled().BindRaw(this, &SGeneratePakTab::HandleGenerateProcessCanceled);
	GeneratePakProcess->OnCompleted().BindRaw(this, &SGeneratePakTab::HandleGenerateProcessCompleted);
	GeneratePakProcess->OnOutput().BindStatic(&SGeneratePakTab::HandleGenerateProcessOutput);

	GeneratePakProcess->Launch();

	++WaitGenerateIndex;
}

void SGeneratePakTab::FinishGenerate()
{
	bIsRunning = false;
	WaitGenerateIndex = 0;
	WaitGeneratePakDatas.Empty();
	GeneratePakProcess.Reset();
}

void SGeneratePakTab::OnGenerate()
{
	if (GeneratePakProcess.IsValid() || !WaitGeneratePakDatas.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::OkCancel, FText::FromString(TEXT("已经有 UnrealPak 任务正在执行中，请等待~~~")));
		return;
	}
	
	// GeneratePakProcess = MakeShared<FMonitoredProcess>(UnrealPakPath)
	
	WaitGeneratePakDatas = MainGeneratePak->GetAllGenereateDatas();
	WaitGenerateIndex = 0;

	bIsRunning = true;
	
	StartGenerate();
}

void SGeneratePakTab::HandleGenerateProcessCanceled()
{
	UE_LOG(LogTemp, Warning, TEXT("Generate process canceled"));

	FinishGenerate();
}

void SGeneratePakTab::HandleGenerateProcessCompleted(int32 ReturnCode)
{
	if (ReturnCode != 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Generate process Failed, Pls Check !!!!!!!!!!!"));
	}
	
	GeneratePakProcess.Reset();
}

void SGeneratePakTab::HandleGenerateProcessOutput(FString Output)
{
	if (!Output.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("Generate Process: %s"), *Output);
	}
}
