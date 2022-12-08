// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickAssetAction.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

void UQuickAssetAction::DuplicateAssets(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		Print(TEXT("Pleasse Enter a VALID Number"), FColor::Red);
		return;
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 Counter = 0;

	for (const FAssetData& SelectedAssetData:SelectedAssetsData)
	{
		for (int32 i = 0; i<NumOfDuplicates; i++)
		{
			const FString SourceAssetPath = SelectedAssetData.ObjectPath.ToString();
			//PrintLog(TEXT("Source Path : ")+SourceAssetPath);
			const FString NewDuplicatedAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i+1);
			//PrintLog(TEXT("New Duplicated Asset Name : ")+NewDuplicatedAssetName);
			const FString NewPathName = FPaths::Combine(SelectedAssetData.PackagePath.ToString(),NewDuplicatedAssetName);
			//PrintLog(TEXT("New Path Name : ")+NewPathName);

			if (UEditorAssetLibrary::DuplicateAsset(SourceAssetPath,NewPathName))
			{
				UEditorAssetLibrary::SaveAsset(NewPathName,false);
				++Counter;
			}
		}
	}

	if (Counter>0)
	{
		Print(TEXT("Successfully duplicated " + FString::FromInt(Counter) + " files"),FColor::Green);
	}
}
