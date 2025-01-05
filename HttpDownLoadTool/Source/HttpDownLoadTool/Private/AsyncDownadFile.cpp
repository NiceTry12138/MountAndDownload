// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncDownadFile.h"
#include "HttpDownloadManagerComponent.h"
#include "HttpModule.h"
#include "MeshPassProcessor.h"
#include "SubTask.h"
#include "Kismet/GameplayStatics.h"


UAsyncDownloadFile::UAsyncDownloadFile(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
}

UAsyncDownloadFile* UAsyncDownloadFile::DownLoadHttpFile(UObject* InWorldContext, const FDownloadData& InData)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(InWorldContext);
	if (!IsValid(GameInstance))
	{
		return nullptr;
	}
	
	UAsyncDownloadFile* HttpFile = NewObject<UAsyncDownloadFile>();
	UHttpDownloadManagerSubsystem* Manager = GameInstance->GetSubsystem<UHttpDownloadManagerSubsystem>();
	
	if (!IsValid(HttpFile) || !IsValid(Manager))
	{
		return nullptr;
	}

	auto Settings = UHttpDownloadManagerSettings::Get();
	HttpFile->URL = InData.Url;
	HttpFile->FileName = InData.FileName;
	HttpFile->State = EHttpDownloadState::Ready;
	HttpFile->bClearCache = InData.bClearCache;
	HttpFile->MD5Hash = FMD5::HashAnsiString(*InData.Url);
	HttpFile->Manager = Manager;
	HttpFile->CacheDirectory = FPaths::Combine(Settings->CacheDirectory, HttpFile->MD5Hash);
	HttpFile->SaveDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), InData.Directory);

#if WITH_EDITORONLY_DATA
	HttpFile->WorldContext = InWorldContext;
#endif
	
	Manager->AddHttpDownloadFile(HttpFile);

	return HttpFile;
}

void UAsyncDownloadFile::Activate()
{
	Super::Activate();

#if WITH_EDITORONLY_DATA
	if (IsValid(WorldContext))
	{
		UKismetSystemLibrary::PrintString(WorldContext, TEXT("---------------------- UAsyncDownloadFile Auto Activate ----------------------"));
		UE_LOG(LogTemp, Warning, TEXT("---------------------- UAsyncDownloadFile Auto Activate ----------------------"));
	}
#endif
}

EHttpDownloadState UAsyncDownloadFile::GetState() const
{
	return State;
}

int32 UAsyncDownloadFile::GetTryCount() const
{
	return TryCount;
}

void UAsyncDownloadFile::StartDownload()
{
	State = EHttpDownloadState::Downloading;
	// 通过 Http 请求 Header 中的 Range:bytes=0-1 获取请求文件的大小
	if (URL.Len() > 0 && FileName.Len() > 0)
	{
		RequestFileSize();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error: Url (%s) or FileName (%s) is empty"), *URL, *FileName);
	}
	++TryCount;
}

void UAsyncDownloadFile::DownloadSuccess()
{
	TArray<uint8> TotalData;
	for (const auto& SubTask : SubTasks)
	{
		if (SubTask->RawData.Num() != SubTask->Size)
		{
			SubTask->LoadData();
		}
		TotalData.Append(MoveTemp(SubTask->RawData));
	}

	FString FileSaveFullPath = FPaths::Combine(SaveDirectory, FileName);
	FFileHelper::SaveArrayToFile(TotalData, *FileSaveFullPath);
	UE_LOG(LogTemp, Log, TEXT("AsyncDownloadFile Download Success, Save File To -> %s"), *FileSaveFullPath);

	State = EHttpDownloadState::Success;
	OnSuccess.Broadcast(FileSaveFullPath, 100);

	if (IsValid(Manager))
	{
		Manager->NotifySucceeded(this);
	}
}

void UAsyncDownloadFile::DownloadFailed()
{
	UE_LOG(LogTemp, Error, TEXT("Can't Request File Total Size ->\r\n \t Url = %s \r\n \t Retry Count = %d"), *URL, TryCount);
	auto Settings = UHttpDownloadManagerSettings::Get();
	if (TryCount <= Settings->MaxTryCount)
	{
		State = EHttpDownloadState::Retry;
	}else
	{
		State = EHttpDownloadState::Failed;
		OnSuccess.Broadcast(TEXT("Failed"), 0);
	}
	if (IsValid(Manager))
	{
		Manager->NotifyFaild(this);
	}
}

void UAsyncDownloadFile::DownloadBegin()
{
	CreateSubTask();
	UpdateTask();
}

void UAsyncDownloadFile::RequestFileSize()
{
	FileSizeRequest = FHttpModule::Get().CreateRequest();
	if (FileSizeRequest)
	{
		FileSizeRequest->SetURL(URL);
		FileSizeRequest->SetVerb(TEXT("HEAD"));
		// https://blog.csdn.net/lotluck/article/details/78486279
		// HTTP中的Range就是分段请求字节数 Range: bytes=0-0 可以探测获取文件大小
		// FileSizeRequest->SetHeader(TEXT("Range"), TEXT("bytes=0-0"));
		FileSizeRequest->OnProcessRequestComplete().BindUObject(this, &UAsyncDownloadFile::ReponseFileSize);
		FileSizeRequest->ProcessRequest();
	}
}

void UAsyncDownloadFile::ReponseFileSize(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccessed)
{
	// 根据重试次数 修改下载状态
	bool bRequestTotalSizeSuccess = false;
	do
	{
		if (!bSuccessed || !HttpResponse.IsValid())
		{
			break;
		}

		int32 Code = HttpResponse->GetResponseCode();
		UE_LOG(LogTemp, Warning, TEXT("ReponseFileSize -> HttpResponse Code -> %d"), Code);

		if (Code != 200)
		{
			break;
		}

		FString RangeStr = HttpResponse->GetHeader("Content-Length");
		if (!RangeStr.IsEmpty())
		{
			TotalFileSize = FCString::Atoi(*RangeStr);
			bRequestTotalSizeSuccess = true;
		}
	}while (false);

	if (!bRequestTotalSizeSuccess || TotalFileSize <= 0)
	{
		DownloadFailed();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Request File \r\n \t URL = %s \r\n \t FileCount = %lld"), *URL, TotalFileSize);
	FileSizeRequest.Reset();
	DownloadBegin();
}

void UAsyncDownloadFile::CreateSubTask()
{
	auto Settings = UHttpDownloadManagerSettings::Get();
	
	const int32 TaskSize = Settings->RequestKBSize * 1024;
	int64 StartSize = 0;
	int64 TaskID = 0;

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	TArray<int64> SuccessSubTasks;
	TArray<FString> ExistedFiles;

	// 如果文件夹存在 则表示曾经下载过该链接的文件
	if (PlatformFile.DirectoryExists(*CacheDirectory))
	{
		PlatformFile.FindFiles(ExistedFiles, *CacheDirectory, DOWNLOAD_SUBTASK_FILE_SUFFIX);
		for (int64 Index = 0; Index < ExistedFiles.Num(); ++Index)
		{
			FString RStr;
			// 理论上缓存的文件名称为 MD5Hash + "_" + TaskId + ".hcf" 所以 RStr 得到的就是 TaskId LStr 得到的 MD5Hash
			ExistedFiles[Index].Split(TEXT("_"), nullptr, &RStr, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			RStr.RemoveFromEnd(DOWNLOAD_SUBTASK_FILE_SUFFIX);	// 去除文件后缀 "。hcf"
			SuccessSubTasks.Add(FCString::Atoi(*RStr));
		}
	}
	while (StartSize < TotalFileSize)
	{
		// 创建子任务
		int64 TempSize = 0;
		FSubHttpTaskPtr SubHttpTask = MakeShareable(new FSubHttpTask);
		SubHttpTask->TaskId = TaskID;
		SubHttpTask->URL = URL;
		SubHttpTask->MD5Hash = MD5Hash;
		SubHttpTask->StartPos = StartSize;
		SubHttpTask->CurFilePath = CacheDirectory;

		FString TempRange;
		if (StartSize + TaskSize <= TotalFileSize)
		{
			TempSize = TaskSize;
			TempRange = FString::Printf(TEXT("bytes=%lld-%lld"), StartSize, StartSize + TempSize - 1);
		}
		else
		{
			TempSize = TotalFileSize - StartSize;
			TempRange = FString::Printf(TEXT("bytes=%lld-"), StartSize);
		}

		UE_LOG(LogTemp, Log, TEXT(" \r\n \t Download SubTask \t TaskId = %lld, TaskRange = %s, Size = %lld, URL = %s"), TaskID, *TempRange, TempSize, *URL);
		
		SubHttpTask->Size = TempSize;
		SubHttpTask->Range = TempRange;
		SubTasks.Add(SubHttpTask);

		if (SuccessSubTasks.Contains(TaskID))
		{
			SubHttpTask->bFinished = true;
			SubHttpTask->bWaitResponse = false;
			CurFileSize += TempSize;
			Progress = CurFileSize * 1.0f / TotalFileSize;
			// 等到写入的时候再读取 此时读取没有必要
			// SubHttpTask->LoadData();
		} else
		{
			++WaitRunTaskNum;
		}
		
		StartSize += TempSize;
		++TaskID;
	}

	if (IsAllSubTaskFinished())
	{
		// 本地存在緩存 直接從本地處理緩存數據即可 無需重新下載
		DownloadSuccess();
	}
}

void UAsyncDownloadFile::UpdateTask()
{
	// 开始执行 请求
	for (const auto& SubTask : SubTasks)
	{
		if (RunningTaskNum < MaxTaskNum && !SubTask->bFinished && !SubTask->bWaitResponse)
		{
			StartSubTask(SubTask);
			++RunningTaskNum;
		}
	}
}

void UAsyncDownloadFile::StartSubTask(FSubHttpTaskPtr SubTask)
{
	auto Request = SubTask->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UAsyncDownloadFile::ResponseSubTask, SubTask->TaskId);
	Request->ProcessRequest();
}

void UAsyncDownloadFile::ResponseSubTask(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccessed,
	int32 TaskID)
{
	FSubHttpTaskPtr SubTask = SubTasks[TaskID];

	bool bRequestSuccess = false;
	do
	{
		if (!bSuccessed || !HttpResponse.IsValid())
     	{
     		break;
     	}
     
     	int32 Code = HttpResponse->GetResponseCode();
     	if (!EHttpResponseCodes::IsOk(Code))
     	{
     		break;
     	}
		
		auto RawData = HttpResponse->GetContent();
		if (RawData.Num() != SubTask->Size)
		{
			break;
		}
		SubTask->RawData = MoveTemp(RawData);
		
		bRequestSuccess = true;
		SubTask->bFailed = false;
		SubTask->SaveData();
		
		CurFileSize += SubTask->Size;
		Progress = CurFileSize * 1.0f / TotalFileSize;
	}while (false);

	SubTask->bFinished = true;
	SubTask->RequestPtr.Reset();

	// 请求失败
	if(!bRequestSuccess)
	{
		SubTask->bFailed = true;
		UE_LOG(LogTemp, Warning, TEXT("AsyncDownloadFile SubTask Failed URL = %s, TaskId = %d "), *URL, TaskID);
	}
	
	--WaitRunTaskNum;
	--RunningTaskNum;

	if (IsAllSubTaskFinished())
	{
		// 全部下载完毕 将所有子任务生成的单个文件拼成整个
		DownloadSuccess();
	}
	else
	{
		UpdateTask();
	}
}

inline bool UAsyncDownloadFile::IsAllSubTaskFinished() const
{
	// 檢查一遍 是否全部成功
	return WaitRunTaskNum <= 0;
}
