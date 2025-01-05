// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DownloadData.generated.h"

#define DOWNLOAD_SUBTASK_FILE_SUFFIX TEXT(".hcf")

UENUM(BlueprintType)
enum class EHttpDownloadState : uint8
{
	Ready = 0 UMETA(DisplayName = "准备中"),
	Downloading = 1 UMETA(DisplayName = "下载中"),
	Success = 2 UMETA(DisplayName = "下载成功"),
	Failed = 3 UMETA(DisplayName = "下载失败"),
	Retry = 4 UMETA(DisplayName = "正在重试"),
};


USTRUCT(BlueprintType)
struct FDownloadData
{
	GENERATED_BODY()

	// 将要下载的路径
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Url;

	// 将要保存的文件名称
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString FileName;

	// 将要保存的文件路径
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Directory;

	// 下载完毕之后是否清除下载文件
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bClearCache = false;
};

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="DownloadMgrSettings"))
class UHttpDownloadManagerSettings : public UDeveloperSettings
{
public:
	GENERATED_BODY()
	
	UHttpDownloadManagerSettings();

	static UHttpDownloadManagerSettings* Get();
	
public:
	// 最大的下载并行数量
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category="Config|HttpManager")
	int32 MaxParallel = 5;	

	// 最大重试数
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category="Config|HttpManager")
	int32 MaxTryCount = 10;

	// 每个子任务下载大小 默认大小为 1 m
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category="Config|HttpManager")
	int32 RequestKBSize = 1024;
	
	// 默认请求超时时间
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category="Config|HttpManager")
	int32 ResponseTimeOut = 5;

	// 子文件的缓存位置
	UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category="Config|HttpManager")
	FString CacheDirectory;
};