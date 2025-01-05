// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "DownloadData.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "SubTask.h"
#include "AsyncDownadFile.generated.h"

class UHttpDownloadManagerSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHttpDownloadDelegate, const FString&, DestPathOrMsg, float, DownloadProgress);

UCLASS()
class HTTPDOWNLOADTOOL_API UAsyncDownloadFile : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	 * @param WorldConxt World 的上下文信息 
	 * @param Url 下载的Http路径
	 * @param InDirectory 文件名
	 * @param InFileName 路径
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category = "HttpDownloader", meta=(WorldContext = "WorldContextObject"))
	static UAsyncDownloadFile* DownLoadHttpFile(UObject* InWorldContext, const FDownloadData& InData);

public:
	virtual void Activate() override;

	EHttpDownloadState GetState() const;

	int32 GetTryCount() const;

	// 开始下载前的准备
	void StartDownload();

protected:
	// 真正开始下载
	void DownloadBegin();
	void DownloadFailed();
	void DownloadSuccess();


protected:
	// 请求文件大小
	void RequestFileSize();
	void ReponseFileSize(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccessed);

	// 创建下载子任务
	void CreateSubTask();

	// 更新子任务
	void UpdateTask();

	//
	void StartSubTask(FSubHttpTaskPtr SubTask);
	void ResponseSubTask(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccessed, int32 TaskID);

	inline bool IsAllSubTaskFinished() const;
public:
	
	UPROPERTY(BlueprintAssignable)
	FHttpDownloadDelegate OnSuccess;

	// 如果已经存在多个下载任务，此时无法下载，于是处于等待状态
	UPROPERTY(BlueprintAssignable)
	FHttpDownloadDelegate OnWait;
	
	UPROPERTY(BlueprintAssignable)
	FHttpDownloadDelegate OnProgress;
	
	UPROPERTY(BlueprintAssignable)
	FHttpDownloadDelegate OnCancel;
	
	UPROPERTY(BlueprintAssignable)
	FHttpDownloadDelegate OnFail;
	
protected:
	// 下载任务状态
	UPROPERTY(BlueprintReadOnly, Transient, Category = "HttpDownloader")
	EHttpDownloadState State = EHttpDownloadState::Ready;

	// 最大尝试次数
	UPROPERTY(BlueprintReadOnly, Transient, Category = "HttpDownloader")
	int32 TryCount = 0;

	// 下载路径
	UPROPERTY(BlueprintReadOnly, Transient, Category = "HttpDownloader")
	FString URL;

	// 文件名称
	UPROPERTY(BlueprintReadOnly, Transient, Category = "HttpDownloader")
	FString FileName;

	// 文件缓存路径
	UPROPERTY(BlueprintReadOnly, Transient, Category = "HttpDownloader")
	FString CacheDirectory;

	// 文件路径
	UPROPERTY(BlueprintReadOnly, Transient, Category = "HttpDownloader")
	FString SaveDirectory;
	
	// 下载进度
	UPROPERTY(BlueprintReadOnly, Transient, Category = "HttpDownloader")
	float Progress = 0.0f;

	// 当前已经下载的文件大小
	UPROPERTY(BlueprintReadOnly, Transient, Category = "HttpDownloader")
	int32 CurFileSize = 0;

	// 文件总大小
	UPROPERTY(BlueprintReadOnly, Transient, Category = "HttpDownloader")
	int64 TotalFileSize = 0;

	// 最大拆分任务数量
	UPROPERTY(BlueprintReadOnly, Transient, Category = "HttpDownloader")
	int32 MaxTaskNum = 5;

	// MD5 可以通过 MD5 生成临时文件名，通过 URL 生成 MD5，保证断点续传时文件名称相同并且不同的URL的文件名称不同
	UPROPERTY(BlueprintReadOnly, Transient, Category = "HttpDownloader")
	FString MD5Hash;

	// 下载完毕之后是否清理缓存
	UPROPERTY(BlueprintReadOnly, Transient, Category = "HttpDownloader")
	bool bClearCache;

	// 等待网络请求的 Task 个数
	int32 WaitRunTaskNum = 0;
	// 正在执行的 Task 个数
	int32 RunningTaskNum = 0;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	UObject* WorldContext = nullptr;
#endif

	UPROPERTY()
	UHttpDownloadManagerSubsystem* Manager = nullptr;

	TSharedPtr<IHttpRequest> FileSizeRequest;

	TArray<FSubHttpTaskPtr> SubTasks;
};
