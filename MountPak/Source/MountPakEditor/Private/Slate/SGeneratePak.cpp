// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/SGeneratePak.h"
#include "DesktopPlatformModule.h"
#include "Slate/SAddedPakList.h"
#include "Tool/GenerateToolLibrary.h"

int64 SGeneratePak::GeneratePakID = 0;

void SGeneratePak::Construct(const FArguments& InArgs)
{
	OnGenerateAllPak = InArgs._OnGenerateAllPak;
	
	SetOrientation(Orient_Vertical);
	InitWidget();
	InitData();
}

const TArray<FGeneratePakItemData>& SGeneratePak::GetAllGenereateDatas() const
{
	return GeneratePakLists;
}

TSharedRef<SVerticalBox> SGeneratePak::CreatePakItemConfigSlate()
{
	TArray<TSharedRef<SHorizontalBox>> HBoxs;
	
	// 选择文件夹
	TxtInputDirectory = SNew(STextBlock).Text(FText::FromString(TEXT("输入需要打包成Pak的文件路径")));
	TSharedRef<SHorizontalBox> HBoxSelectInputDirectory = SNew(SHorizontalBox);
	HBoxs.Add(HBoxSelectInputDirectory);
	HBoxSelectInputDirectory->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SButton)
			.Text(FText::FromString(TEXT("选择输入文件夹")))
			.OnClicked(this, &SGeneratePak::OnInputDirectoryButtonClicked)
		];
	
	HBoxSelectInputDirectory->AddSlot()
		.FillWidth(1.0f)
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			TxtInputDirectory.ToSharedRef()
		];
	
	// 选择输出文件夹
	TxtOutputDirectory = SNew(STextBlock).Text(FText::FromString(TEXT("输入保存Pak文件的路径")));
	TSharedRef<SHorizontalBox> HBoxSelectOutputDirectory = SNew(SHorizontalBox);
	HBoxs.Add(HBoxSelectOutputDirectory);
	HBoxSelectOutputDirectory->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SButton)
			.Text(FText::FromString(TEXT("选择输出文件夹")))
			.OnClicked(this, &SGeneratePak::OnOutputDirectoryButtonClicked)
		];
	
	HBoxSelectOutputDirectory->AddSlot()
		.FillWidth(1.0f)
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			TxtOutputDirectory.ToSharedRef()
		];

	// 输入输出 Pak 文件名称
	TxtPakFileName = SNew(SEditableText)
						.HintText(FText::FromString(TEXT("Pak文件名称")));
	TSharedRef<SHorizontalBox> HBoxPakFileName = SNew(SHorizontalBox);
	HBoxs.Add(HBoxPakFileName);
	HBoxPakFileName->AddSlot()
		.FillWidth(1.0f)
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			TxtPakFileName.ToSharedRef()
		];
	
	// 是否压缩的 CheckBox
	CompressCombox = SNew(SCheckBox)
						.ToolTipText(FText::FromString(TEXT("是否要压缩")))
						.Type(ESlateCheckBoxType::CheckBox)
						.IsChecked(ECheckBoxState::Checked);
	TSharedRef<SHorizontalBox> HBoxCheckCompress = SNew(SHorizontalBox);
	HBoxs.Add(HBoxCheckCompress);
	HBoxCheckCompress->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(STextBlock)
				.Text(FText::FromString(TEXT("是否压缩")))
		];
	HBoxCheckCompress->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			CompressCombox.ToSharedRef()
		];
	
	auto Result = SNew(SVerticalBox);

	for (const auto& HBoxItem : HBoxs)
	{
		Result->AddSlot()
		.AutoHeight()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			HBoxItem
		];
	}
	
	return Result;
}

void SGeneratePak::InitData()
{
	// 直接定位到 Cooked 文件夹
	LastInputDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Cooked"));
	LastInputDirectory = FPaths::ConvertRelativePathToFull(LastInputDirectory);

	// 直接定位到 Cooked 文件夹
	LastOutputDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("MountPak"));
	LastOutputDirectory = FPaths::ConvertRelativePathToFull(LastOutputDirectory);
	
}

void SGeneratePak::InitWidget()
{
	// 标题
	AddSlot()
		.AutoSize()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
					.Text(FText::FromString(TEXT("生成 Pak 文件")))
			]
		];


	// 添加一个生成项
	AddSlot()
		.AutoSize()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				CreatePakItemConfigSlate()
			]

			+ SHorizontalBox::Slot()
			.FillWidth(0.3f)
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("添加到待生成列表")))
				.OnClicked(this, &SGeneratePak::OnAddSpawnListButtonClicked)
			]
		];

	// 添加显示用的 ListView
	AddedPakList = SNew(SAddedPakList).OnItemDataDelete(this, &SGeneratePak::OnDeleteItemClicked);
	
	AddSlot()
	.FillSize(1.0f)
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		AddedPakList.ToSharedRef()
	];


	// 添加生成按钮
	AddSlot()
	.AutoSize()
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SButton)
		.Text(FText::FromString(TEXT("开始生成 Pak")))
		.OnClicked(this, &SGeneratePak::OnGenerateAllButtonClicked)
	];
}

bool SGeneratePak::CheckItemData(const FGeneratePakItemData& ItemData)
{
	if (ItemData.SaveFileName.IsEmpty() || ItemData.SaveDirectory.IsEmpty() || ItemData.InDirectory.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::OkCancel, FText::FromString(TEXT("请检查保存 Pak 文件名、输入文件夹、保存文件夹是否为空")));
		return false;
	}

	FString InItemSaveFile = FPaths::Combine(ItemData.SaveDirectory, ItemData.SaveFileName);
	if (!InItemSaveFile.EndsWith(TEXT(".pak")))
	{
		InItemSaveFile += TEXT(".pak");
	}
	
	for (const auto& AddedItem : GeneratePakLists)
	{
		FString AddedSaveFile = FPaths::Combine(AddedItem.SaveDirectory, AddedItem.SaveFileName);
		if (InItemSaveFile.Equals(AddedSaveFile))
		{
			EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString(TEXT("已经有相同 输出路径+文件名 的配置存在，是否仍然添加？\r\n如果选择是，则将把多个输入路径下的文件打包到同一个 pak 文件中")));
			if (Result == EAppReturnType::No)
			{
				return false;
			}
			else
			{
				// 既然已经确定添加 那么直接全部跳过检查了 万一 100 个重复的项 要点 100 次确定 有点恶心
				return true;
			}
		}
	}
	
	return true;
}

void SGeneratePak::DeletePakItemData(int InItemId)
{
	int DelIndex = -1;
	for (int Index = 0; Index < GeneratePakLists.Num(); Index++)
	{
		if (GeneratePakLists[Index].ItemID == InItemId)
		{
			DelIndex = Index;
			break;
		}
	}
	if (DelIndex < 0 || DelIndex >= GeneratePakLists.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Delete PakItemData With Id = %d"), InItemId);
		return;
	}
	GeneratePakLists.RemoveAt(DelIndex);
}

FReply SGeneratePak::OnInputDirectoryButtonClicked()
{
	FString OutFolderName;
	
	if (LastInputDirectory.IsEmpty())
	{
		// 直接定位到 Cooked 文件夹
		LastInputDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Cooked"));
	}

	// 默认保存 Pak 文件的文件夹
	if (!FPaths::DirectoryExists(LastOutputDirectory))
	{
		// 如果文件夹不存在表示没有 Cook Content 文件
		FMessageDialog::Open(EAppMsgType::OkCancel, FText::FromString(TEXT("未找到 Saved/Cooked 文件夹，请先 Cook 资产到指定平台")));
		return FReply::Handled();
	}

	if (!FDesktopPlatformModule::Get()->OpenDirectoryDialog(FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr), TEXT(""), LastInputDirectory, OutFolderName))
	{
		return FReply::Handled();
	}
	
	TxtInputDirectory->SetText(FText::FromString(OutFolderName));
	
	return FReply::Handled();
}

FReply SGeneratePak::OnOutputDirectoryButtonClicked()
{
	FString OutFolderName;

	if (LastOutputDirectory.IsEmpty())
	{
		// 直接定位到 Cooked 文件夹
		LastOutputDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("MountPak"));
	}

	// 默认保存 Pak 文件的文件夹
	if (!FPaths::DirectoryExists(LastOutputDirectory))
	{
		bool bSuccess = IFileManager::Get().MakeDirectory(*LastOutputDirectory, true);
		if (!bSuccess)
		{
			UE_LOG(LogTemp, Error, TEXT("Create Default MountPak Save Directory at %s but failed."), *LastOutputDirectory);
		}
	}
	
	if (!FDesktopPlatformModule::Get()->OpenDirectoryDialog(FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr), TEXT(""), LastOutputDirectory, OutFolderName))
	{
		return FReply::Handled();
	}

	TxtOutputDirectory->SetText(FText::FromString(OutFolderName));
	
	return FReply::Handled();
}

FReply SGeneratePak::OnAddSpawnListButtonClicked()
{
	bool bIsCompress = CompressCombox->IsChecked();
	FString InputFileDirectory = TxtInputDirectory->GetText().ToString();
	FString OutputFileDirectory = TxtOutputDirectory->GetText().ToString();
	FString SaveFileName = TxtPakFileName->GetText().ToString();

	FGeneratePakItemData InData;
	InData.bIsCompress = bIsCompress;
	InData.InDirectory = InputFileDirectory;
	InData.SaveDirectory = OutputFileDirectory;
	InData.SaveFileName = SaveFileName;
	
	// 检查待生成列表中是否存在相同输出路径并且相同名称的配置
	if (!CheckItemData(InData))
	{
		return FReply::Handled();
	}

	if (!InData.SaveFileName.EndsWith(TEXT(".pak")))
	{
		InData.SaveFileName += TEXT(".pak");
	}

	GeneratePakLists.Add(InData);
	AddedPakList->Refresh(GeneratePakLists);

	TxtInputDirectory->SetText(FText::GetEmpty());
	TxtOutputDirectory->SetText(FText::GetEmpty());
	TxtPakFileName->SetText(FText::GetEmpty());

	// 方便文件夹选择
	LastInputDirectory = InputFileDirectory;
	LastOutputDirectory = OutputFileDirectory;
	
	return FReply::Handled();
}

FReply SGeneratePak::OnGenerateAllButtonClicked() const
{
	if (OnGenerateAllPak.IsBound())
	{
		OnGenerateAllPak.Execute();
	}
	return FReply::Handled();
}

void SGeneratePak::OnDeleteItemClicked(int InItemId)
{
	DeletePakItemData(InItemId);
	AddedPakList->Refresh(GeneratePakLists);
}
