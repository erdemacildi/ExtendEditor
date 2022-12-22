// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AdvanceDeletionWidget.h"

#include "Widgets/Layout/SScrollBox.h"
#include "SlateBasics.h"

void SAdvanceDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	StoredAssetsData = InArgs._AssetsDataToStore;

	FSlateFontInfo TitleTextFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TitleTextFont.Size = 30;

	FSlateFontInfo TitleTextFont2 = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TitleTextFont.Size = 24;

	FSlateFontInfo TitleTextFont3 = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TitleTextFont.Size = 45;

	ChildSlot
	[   //Main Vertical Box
		SNew(SVerticalBox)

		//First vertical slot for title text
		+SVerticalBox::Slot()
		.AutoHeight()[
			SNew(STextBlock)
				.Text(FText::FromString(TEXT("Advance Deletion")))
				.Font(TitleTextFont)
				.Justification(ETextJustify::Center)
				.ColorAndOpacity(FColor::White)
		]

		//SecondSlot for drop down to specify the listing condition
		+SVerticalBox::Slot()
		.AutoHeight()[
			SNew(STextBlock)
				.Text(FText::FromString(TEXT("A drop down for listing condition")))
				.Font(TitleTextFont2)
				.Justification(ETextJustify::Left)
				.ColorAndOpacity(FColor::White)
		]

		//Third slot for the actual asset list
		+SVerticalBox::Slot()
		.AutoHeight()[
			SNew(SScrollBox)

			+SScrollBox::Slot()[
				SNew(SListView<TSharedPtr<FAssetData>>)
				.ItemHeight(24.f)
				.ListItemsSource(&StoredAssetsData)
				.OnGenerateRow(this,
					&SAdvanceDeletionTab::OnGenerateRowForList)
				]
		]

		//Fourth slot for 3 buttons
		+SVerticalBox::Slot()
		.AutoHeight()[
			SNew(SHorizontalBox)
		]
	];
}

TSharedRef<ITableRow> SAdvanceDeletionTab::OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();
	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget = 
	SNew(STableRow<TSharedPtr<FAssetData>>,OwnerTable)[
		SNew(STextBlock)
		.Text(FText::FromString(DisplayAssetName))];
	return ListViewRowWidget;
}
