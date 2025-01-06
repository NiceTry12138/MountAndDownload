// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MOUNTPAKEDITOR_API GenerateToolLibrary
{
public:
	/**
	 * 保存 InputDirect 路径下所有文件的打包信息到 PakFileName 同名的 txt 文件中，并返回 txt 文件的路径
	 * @param PakFileName 输入保存的 Pak 文件名，以此名称保存 txt 配置文件
	 * @param InputDirect 输入文件夹路径，生成该文件夹下所有文件到 txt 配置文件中
	 * @param bCompress 是否压缩
	 * @return 生成的 txt 配置文件的路径
	 */
	static FString GeneratePakConfigTxt(const FString& PakFileName, const TArray<FString>& InputDirects, bool bCompress);
};
