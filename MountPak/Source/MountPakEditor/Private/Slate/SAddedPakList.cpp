// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/SAddedPakList.h"
#include "Widgets/Input/SButton.h"

const FName ItemData_SaveFileName(TEXT("文件名称"));
const FName ItemData_InputDirectory(TEXT("输入路径"));
const FName ItemData_OutputDirectory(TEXT("输出路径"));
const FName ItemData_Compress(TEXT("是否压缩"));
const FName ItemData_Operator(TEXT("操作按钮"));


void SGeneratePakItemDataViewRow::Construct(const FArguments& Args, const TSharedRef<STableViewBase>& InOwnerTable, FGeneratePakItemDataPtr InElementInfo)
{
	OnItemDataDeleteHandle = Args._OnItemDataDelete;
	ElementInfo = InElementInfo;
	SMultiColumnTableRow<FGeneratePakItemDataPtr>::Construct(FSuperRowType::FArguments(), InOwnerTable);
}

FReply SGeneratePakItemDataViewRow::OnDeleteItemClicked() const
{
	if (OnItemDataDeleteHandle.IsBound())
	{
		OnItemDataDeleteHandle.Execute(ElementInfo->ItemID);
	}
	return FReply::Handled();
}

TSharedRef<SWidget> SGeneratePakItemDataViewRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	
	if (ColumnName.IsEqual(ItemData_SaveFileName))
	{
		return SNew(STextBlock).Text(FText::FromString(ElementInfo->SaveFileName));
	}
	else if (ColumnName.IsEqual(ItemData_InputDirectory))
	{
		return SNew(STextBlock).Text(FText::FromString(ElementInfo->InDirectory));
	}
	else if (ColumnName.IsEqual(ItemData_OutputDirectory))
	{
		return SNew(STextBlock).Text(FText::FromString(ElementInfo->SaveDirectory));
	}
	else if (ColumnName.IsEqual(ItemData_Compress))
	{
		FString Compress = ElementInfo->bIsCompress ? TEXT("压缩") : TEXT("不压缩");
		return SNew(STextBlock).Text(FText::FromString(Compress));
	}
	else if (ColumnName.IsEqual(ItemData_Operator))
	{
		return SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("删除")))
				.OnClicked(this, &SGeneratePakItemDataViewRow::OnDeleteItemClicked)
			];
	}

	return SNullWidget::NullWidget;
}

void SAddedPakList::Construct(const FArguments& Args)
{
	OnItemDataDeleteHandle = Args._OnItemDataDelete;
	SListView::Construct(
		SListView::FArguments()
		.SelectionMode(ESelectionMode::None)
		.ListItemsSource(&GeneratePakDatas)
		.OnGenerateRow(this, &SAddedPakList::OnGenerateRowListView)
		.HeaderRow(
			SNew(SHeaderRow)

			+ SHeaderRow::Column(ItemData_InputDirectory)
			.DefaultLabel(FText::FromName(ItemData_InputDirectory))
			.HAlignCell(HAlign_Left)
			
			+ SHeaderRow::Column(ItemData_OutputDirectory)
			.DefaultLabel(FText::FromName(ItemData_OutputDirectory))
			.HAlignCell(HAlign_Left)
			
			+ SHeaderRow::Column(ItemData_SaveFileName)
			.DefaultLabel(FText::FromName(ItemData_SaveFileName))
			.HAlignCell(HAlign_Left)
			
			+ SHeaderRow::Column(ItemData_Compress)
			.DefaultLabel(FText::FromName(ItemData_Compress))
			.HAlignCell(HAlign_Left)
			
			+ SHeaderRow::Column(ItemData_Operator)
			.DefaultLabel(FText::FromName(ItemData_Operator))
			.HAlignCell(HAlign_Left)
		)
	);
}

void SAddedPakList::Refresh(const TArray<FGeneratePakItemData>& InData)
{
	GeneratePakDatas.Empty();

	for (const auto& Item : InData)
	{
		auto ItemDataPtr= MakeShared<FGeneratePakItemData>();
		ItemDataPtr->ItemID = Item.ItemID;
		ItemDataPtr->InDirectory = Item.InDirectory;
		ItemDataPtr->SaveDirectory = Item.SaveDirectory;
		ItemDataPtr->SaveFileName = Item.SaveFileName;
		ItemDataPtr->bIsCompress = Item.bIsCompress;
		
		GeneratePakDatas.Add(ItemDataPtr);
	}

	RequestListRefresh();
}

void SAddedPakList::OnItemDataDeleted(int InItemId) const
{
	if (OnItemDataDeleteHandle.IsBound())
	{
		OnItemDataDeleteHandle.Execute(InItemId);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SAddedPakList OnItemDataDeleteHandle Event Is Not Bound Anything, Pls Check"));
	}
}

TSharedRef<ITableRow> SAddedPakList::OnGenerateRowListView(FGeneratePakItemDataPtr ItemInfo, const TSharedRef<STableViewBase>& InOwnerTable)
{
	return SNew(SGeneratePakItemDataViewRow, InOwnerTable, ItemInfo)
			.OnItemDataDelete(this, &SAddedPakList::OnItemDataDeleted);
}
