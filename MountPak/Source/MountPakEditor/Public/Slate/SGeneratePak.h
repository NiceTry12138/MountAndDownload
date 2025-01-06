// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Input/SCheckBox.h"
#include "GeneratePakData.h"

class SAddedPakList;

DECLARE_DELEGATE(FOnGenerateAllPak);

class MOUNTPAKEDITOR_API SGeneratePak : public SStackBox
{
public:
	SLATE_BEGIN_ARGS(SGeneratePak)
	{
	}
		SLATE_EVENT(FOnGenerateAllPak, OnGenerateAllPak)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	
public:
	const TArray<FGeneratePakItemData>& GetAllGenereateDatas() const;

protected:
	TSharedRef<SVerticalBox> CreatePakItemConfigSlate();
	void InitData();
	void InitWidget();

	bool CheckItemData(const FGeneratePakItemData& ItemData);
	void DeletePakItemData(int InItemId);
	
protected:
	FReply OnInputDirectoryButtonClicked();
	FReply OnOutputDirectoryButtonClicked();
	FReply OnAddSpawnListButtonClicked();
	FReply OnGenerateAllButtonClicked() const;
	void OnDeleteItemClicked(int InItemId);

private:
	TSharedPtr<STextBlock> TxtInputDirectory;
	TSharedPtr<STextBlock> TxtOutputDirectory;
	TSharedPtr<SEditableText> TxtPakFileName;
	TSharedPtr<SCheckBox> CompressCombox;
	TSharedPtr<SAddedPakList> AddedPakList;
	
	FString LastInputDirectory;					// 上次需要生成 Pak 的路径
	FString LastOutputDirectory;				// 上次保存 Pak 的路径
	FString LastPakFileName;					// 上次保存 Pak 的文件名称
	bool bLastCompress = true;					// 上次是否压缩

	static int64 GeneratePakID;					// 生成的 Pak 配置的 ID

	FOnGenerateAllPak OnGenerateAllPak;

	TArray<FGeneratePakItemData> GeneratePakLists;
};
