// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AdvanceDeletionWidget.h"

#include "SuperManager.h"
#include "Widgets/Layout/SScrollBox.h"
#include "SlateBasics.h"
#include "DebugHeader.h"

void SAdvanceDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	StoredAssetsData = InArgs._AssetsDataToStore;

	FSlateFontInfo TitleTextFont = GetEmbossedTextFont();
	TitleTextFont.Size = 30;

	FSlateFontInfo TitleTextFont2 = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TitleTextFont.Size = 24;

	/*FSlateFontInfo TitleTextFont3 = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TitleTextFont.Size = 45;*/

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
		.VAlign(VAlign_Fill)[
			SNew(SScrollBox)

			+SScrollBox::Slot()[
				ConstructAssetListView()
				]
		]

		//Fourth slot for 3 buttons
		+SVerticalBox::Slot()
		.AutoHeight()[
			SNew(SHorizontalBox)

			+SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)[
				ConstructDeleteAllButton()	
			]

			+SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)[
				ConstructSelectAllButton()
			]

			+SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)[
				ConstructDeselectAllButton()
			]
		]
	];
}

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAdvanceDeletionTab::ConstructAssetListView()
{
	ConstructedAssetListView = SNew(SListView<TSharedPtr<FAssetData>>)
				.ItemHeight(24.f)
				.ListItemsSource(&StoredAssetsData)
				.OnGenerateRow(this,
					&SAdvanceDeletionTab::OnGenerateRowForList);

	return ConstructedAssetListView.ToSharedRef();
}

void SAdvanceDeletionTab::RefreshAssetListView()
{
	if (ConstructedAssetListView.IsValid())
	{
		ConstructedAssetListView->RebuildList();
	}
}

#pragma region RowWidgetForAssetListView

TSharedRef<ITableRow> SAdvanceDeletionTab::OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!AssetDataToDisplay.IsValid()) return SNew(STableRow<TSharedPtr<FAssetData>>,OwnerTable);

	const FString DisplayAssetClassName = AssetDataToDisplay->AssetClass.ToString();
	const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();

	FSlateFontInfo AssetClassNameFont = GetEmbossedTextFont();
	AssetClassNameFont.Size = 10;

	FSlateFontInfo AssetNameFont = GetEmbossedTextFont();
	AssetNameFont.Size = 15;
	
	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget = 
		SNew(STableRow<TSharedPtr<FAssetData>>,OwnerTable).Padding(FMargin(5.f))[
			SNew(SHorizontalBox)

			//First slot for check box
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.FillWidth(.05f)[
				ConstructCheckBox(AssetDataToDisplay)
			]

			//Second slot for displaying asset class name
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Fill)
			.FillWidth(0.5f)[
				ConstructTextForRowWidget(DisplayAssetClassName, AssetClassNameFont)
			]

			//Third slot for displaying asset name
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Fill)[
				ConstructTextForRowWidget(DisplayAssetName, AssetNameFont)
			]
			//Fourth slot for a button
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Fill)[
				ConstructButtonForRowWidget(AssetDataToDisplay)
			]
	
		];
	return ListViewRowWidget;
}

TSharedRef<SCheckBox> SAdvanceDeletionTab::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SCheckBox> ConstructedCheckBox =
	SNew(SCheckBox)
	.Type(ESlateCheckBoxType::CheckBox)
	.OnCheckStateChanged(this,&SAdvanceDeletionTab::OnCheckBoxStateChanged,AssetDataToDisplay)
	.Visibility(EVisibility::Visible);

	return ConstructedCheckBox;
}

void SAdvanceDeletionTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
		DebugHeader::Print(AssetData->AssetName.ToString() + TEXT(" is unchecked"),FColor::Red);
		break;
		
	case ECheckBoxState::Checked:
		DebugHeader::Print(AssetData->AssetName.ToString() + TEXT(" is checked"),FColor::Green);
		break;
		
	case ECheckBoxState::Undetermined:
		
		break;
		
	default:
		break;		
	}
	
}

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontToUse)
{
	TSharedRef<STextBlock> ConstructedTextBlock = 
	SNew(STextBlock)
	.Text(FText::FromString(TextContent))
	.Font(FontToUse)
	.ColorAndOpacity(FColor::White);

	return ConstructedTextBlock;
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructButtonForRowWidget(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> ConstructedButton =
	SNew(SButton)
	.Text(FText::FromString(TEXT("Delete")))
	.OnClicked(this,&SAdvanceDeletionTab::OnDeleteButtonClicked,AssetDataToDisplay);

	return ConstructedButton;
}

FReply SAdvanceDeletionTab::OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData)
{
	FSuperManagerModule& SuperManagerModule =
		FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));

	const bool bAssetDeleted = SuperManagerModule.DeleteSingleAssetForAssetList(*ClickedAssetData.Get());

	if (bAssetDeleted)
	{
		//Updating the list source items
		if(StoredAssetsData.Contains(ClickedAssetData))
		{
			StoredAssetsData.Remove(ClickedAssetData);
		}
		//Refresh the list
		RefreshAssetListView();
	}

	return FReply::Handled();
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructDeleteAllButton()
{
	TSharedRef<SButton> DeleteAllButton = SNew(SButton)
	.ContentPadding(FMargin(5.f))
	.OnClicked(this,&SAdvanceDeletionTab::OnDeleteAllButtonClicked);

	DeleteAllButton->SetContent(ConstructTextForTabButtons(TEXT("Delete All")));

	return DeleteAllButton;
}

FReply SAdvanceDeletionTab::OnDeleteAllButtonClicked()
{
	DebugHeader::Print(TEXT("Delete All Button Clicked"),FColor::Cyan);
	return FReply::Handled();
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructSelectAllButton()
{
	TSharedRef<SButton> SelectAllButton = SNew(SButton)
	.ContentPadding(FMargin(5.f))
	.OnClicked(this,&SAdvanceDeletionTab::OnSelectAllButtonClicked);

	SelectAllButton->SetContent(ConstructTextForTabButtons(TEXT("Select All")));

	return SelectAllButton;
}

FReply SAdvanceDeletionTab::OnSelectAllButtonClicked()
{
	DebugHeader::Print(TEXT("Select All Button Clicked"),FColor::Cyan);
	return FReply::Handled();
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructDeselectAllButton()
{
	TSharedRef<SButton> DeselectAllButton = SNew(SButton)
	.ContentPadding(FMargin(5.f))
	.OnClicked(this,&SAdvanceDeletionTab::OnDeselectAllButtonClicked);

	DeselectAllButton->SetContent(ConstructTextForTabButtons(TEXT("Deselect All")));

	return DeselectAllButton;
}

FReply SAdvanceDeletionTab::OnDeselectAllButtonClicked()
{
	DebugHeader::Print(TEXT("Deselect All Button Clicked"),FColor::Cyan);
	return FReply::Handled();
}

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructTextForTabButtons(const FString& TextContent)
{
	FSlateFontInfo ButtonTextFont = GetEmbossedTextFont();
	ButtonTextFont.Size = 15;
	TSharedRef<STextBlock> ConstructedTextBlock = SNew(STextBlock)
	.Text(FText::FromString(TextContent))
	.Font(ButtonTextFont)
	.Justification(ETextJustify::Center);

	return ConstructedTextBlock;
}

#pragma endregion
