#include <SubTask.h>

#include "HttpModule.h"
#include "DownloadData.h"

FSubHttpTask::FSubHttpTask()
{
}

FSubHttpTask::~FSubHttpTask()
{
	Stop();
}

const inline TSharedPtr<IHttpRequest>& FSubHttpTask::CreateRequest()
{
	RequestPtr = FHttpModule::Get().CreateRequest();
	RequestPtr->SetURL(URL);
	RequestPtr->SetVerb(TEXT("GET"));
	RequestPtr->SetHeader(TEXT("Range"), Range);
	bWaitResponse = true;
	RequestTime = FApp::GetCurrentTime();

	return RequestPtr;
}

inline void FSubHttpTask::Stop()
{
	if (RequestPtr.IsValid())
	{
		RequestPtr.Reset();
	}
}

inline void FSubHttpTask::SaveData()
{
	FString SubTaskFileName = MD5Hash + TEXT("_") + FString::FromInt(TaskId) + DOWNLOAD_SUBTASK_FILE_SUFFIX;

	FFileHelper::SaveArrayToFile(RawData, *FPaths::Combine(CurFilePath, SubTaskFileName));

	UE_LOG(LogTemp, Log, TEXT("SubTask Save File -> %s"), *FPaths::Combine(CurFilePath, SubTaskFileName));
}

inline void FSubHttpTask::LoadData()
{
	FString SubTaskFileName = MD5Hash + TEXT("_") + FString::FromInt(TaskId) + DOWNLOAD_SUBTASK_FILE_SUFFIX;

	FFileHelper::LoadFileToArray(RawData, *FPaths::Combine(CurFilePath, SubTaskFileName));
}
