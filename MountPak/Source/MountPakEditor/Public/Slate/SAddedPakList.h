// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeneratePakData.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"

typedef TSharedPtr<FGeneratePakItemData> FGeneratePakItemDataPtr;

DECLARE_DELEGATE_OneParam(FOnItemDataDeleted, int);

class SGeneratePakItemDataViewRow : public SMultiColumnTableRow<FGeneratePakItemDataPtr>
{
public:
	SLATE_BEGIN_ARGS(SGeneratePakItemDataViewRow)
	{
	}
		SLATE_EVENT(FOnItemDataDeleted, OnItemDataDelete)	
	SLATE_END_ARGS()

	void Construct(const FArguments& Args, const TSharedRef<STableViewBase>& InOwnerTable, FGeneratePakItemDataPtr InElementInfo);

protected:
	FReply OnDeleteItemClicked() const;
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

private:
	FOnItemDataDeleted OnItemDataDeleteHandle;
	FGeneratePakItemDataPtr ElementInfo;
};

/**
 * 
 */
class MOUNTPAKEDITOR_API SAddedPakList : public SListView<FGeneratePakItemDataPtr>
{
public:
	SLATE_BEGIN_ARGS(SAddedPakList)
	{
		
	}
		SLATE_EVENT(FOnItemDataDeleted, OnItemDataDelete)
	SLATE_END_ARGS()

	void Construct(const FArguments& Args);

	void Refresh(const TArray<FGeneratePakItemData>& InData);
	
protected:
	void OnItemDataDeleted(int InItemId) const;

	TSharedRef<ITableRow> OnGenerateRowListView(FGeneratePakItemDataPtr ItemInfo, const TSharedRef<STableViewBase>& InOwnerTable);

private:
	TArray<FGeneratePakItemDataPtr> GeneratePakDatas;
	
	FOnItemDataDeleted OnItemDataDeleteHandle;
};
