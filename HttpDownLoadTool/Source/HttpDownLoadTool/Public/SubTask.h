// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
// #include "SubTask.generated.h"

// class UAsyncDownloadFile;

struct FSubHttpTask : TSharedFromThis<FSubHttpTask>
{
public:
	FSubHttpTask();
	~FSubHttpTask();

	int32 TaskId = -1;					// 任务编号
	FString URL;						// 下载链接
	FString MD5Hash;					// 根据链接生成的MD5
	FString CurFilePath;				// 下载路径
	FString Range;						// 范围
	bool bFinished = false;				// 下载是否结束
	bool bFailed = false;				// 下载是否失败
	bool bWaitResponse = false;			// 是否正在等待 Response
	int64 StartPos = -1;				// 下载开始的位置
	int64 Size = -1;					// 下载大小
	TArray<uint8> RawData;				// 原始数据	
	double RequestTime = -1.0f;			// 请求时间
	TSharedPtr<IHttpRequest> RequestPtr;// IHttpRequest 对象

	const TSharedPtr<IHttpRequest>& CreateRequest();
	void Stop();
	void SaveData();
	void LoadData();
};

typedef TSharedPtr<FSubHttpTask> FSubHttpTaskPtr;