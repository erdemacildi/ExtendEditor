// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManager.h"

#include "ContentBrowserModule.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	InitCBMenuExtention();

	RegisterAdvanceDeletionTab();
}

#pragma region ContentBrowserMenuExtention

void FSuperManagerModule::InitCBMenuExtention()
{
	//for adding content browser module 
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	//Get hold of all the menu extenders
	TArray<FContentBrowserMenuExtender_SelectedPaths>& ContentBrowserModuleMenuExtenders = ContentBrowserModule.GetAllPathViewContextMenuExtenders();

	/*FContentBrowserMenuExtender_SelectedPaths CustomCBMenuDelegate;
	CustomCBMenuDelegate.BindRaw(this,&FSuperManagerModule::CustomCBMenuExtender);
	
	ContentBrowserModuleMenuExtenders.Add(CustomCBMenuDelegate);*/

	//We add custom delegate toall the existing delegates
	ContentBrowserModuleMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this,&FSuperManagerModule::CustomCBMenuExtender));
}

//To define the position for inserting menu entry
TSharedRef<FExtender> FSuperManagerModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender (new FExtender());

	if (SelectedPaths.Num()>0)
	{
		MenuExtender->AddMenuExtension(FName("Delete"), //Extention hook, position insert
			EExtensionHook::After, //Inserting before or after
			TSharedPtr<FUICommandList>(), //Custom hot keys
			FMenuExtensionDelegate::CreateRaw(this,&FSuperManagerModule::AddCBMenuEntry)); //Second binding, will define details for this menu entry

		FolderPathsSelected = SelectedPaths;
	}
	
	return MenuExtender;
}

//Define details for the custom menu entry
void FSuperManagerModule::AddCBMenuEntry(FMenuBuilder& MenuBuilder)
{ 
	/*MenuBuilder.AddSubMenu(
		FText::FromString(TEXT("Delete U")),
		FText::FromString(TEXT("Safely delete all unused assets under folders")),
		MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Unused Assets")), //Title text for menu entry
		FText::FromString(TEXT("Safely delete all unused assets under folders")), //Tooltip text
		FSlateIcon(), //Custom icon
		FExecuteAction::CreateRaw(this,&FSuperManagerModule::OnDeleteUnusedAssetButtonClicked)),
		
		);*/
	
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Unused Assets")), //Title text for menu entry
		FText::FromString(TEXT("Safely delete all unused assets under folders")), //Tooltip text
		FSlateIcon(), //Custom icon
		FExecuteAction::CreateRaw(this,&FSuperManagerModule::OnDeleteUnusedAssetButtonClicked)); //The actual function to execute

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Empty Folders")), //Title text for menu entry
		FText::FromString(TEXT("Safely delete all empty folders")), //Tooltip text
		FSlateIcon(), //Custom icon
		FExecuteAction::CreateRaw(this,&FSuperManagerModule::OnDeleteEmptyFoldersButtonClicked)); //The actual function to execute

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Empty Folders and Unused Assets")), //Title text for menu entry
		FText::FromString(TEXT("Safely delete all empty folders and unused assets")), //Tooltip text
		FSlateIcon(), //Custom icon
		FExecuteAction::CreateRaw(this,&FSuperManagerModule::OnDeleteEmptyFoldersAndUnusedAssetsButtonClicked)); //The actual function to execute

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Advance Deltion")), //Title text for menu entry
		FText::FromString(TEXT("List assets by specific condition in a tab for deleting")), //Tooltip text
		FSlateIcon(), //Custom icon
		FExecuteAction::CreateRaw(this,&FSuperManagerModule::OnAdvanceDeletionButtonClicked)); //The actual function to execute
	
}

void FSuperManagerModule::OnDeleteUnusedAssetButtonClicked()
{
	//Must select one folder
	if (FolderPathsSelected.Num()>1)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("You can only do this to one folder"));
		return;
	}

	//Selects all assets in this folder recursively (Contains sub folders)
	TArray<FString> AssetsPathNames = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);

	//Whether there are assets under the folder
	if (AssetsPathNames.Num()==0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No asset found under selected folder"),false);
		return;
	}

	EAppReturnType::Type ConfirmResult =
		DebugHeader::ShowMsgDialog(EAppMsgType::YesNo,TEXT("A total of ") + FString::FromInt(AssetsPathNames.Num()) + TEXT(" assets need to be checked.\nWould you like to proceed?"),false);

	if (ConfirmResult == EAppReturnType::No) return;

	FixUpRedirectors();
	
	TArray<FAssetData> UnusedAssetsDataArray;

	for (const FString& AssetPathName:AssetsPathNames)
	{
		//Don't touch root folders
		if (AssetPathName.Contains(TEXT("Developers")) ||
			AssetPathName.Contains(TEXT("Collections")))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName)) continue;

		TArray<FString> AssetReferencers =
			UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);

		if (AssetReferencers.Num()==0)
		{
			const FAssetData UnusedAssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
			UnusedAssetsDataArray.Add(UnusedAssetData);
		}
	}

	if (UnusedAssetsDataArray.Num()>0)
	{
		ObjectTools::DeleteAssets(UnusedAssetsDataArray);
	}
	else
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No unused asset found under selected folder"),false);
	}
}

void FSuperManagerModule::OnDeleteEmptyFoldersButtonClicked()
{
	FixUpRedirectors();
	
	TArray<FString> FolderPathsArray = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0],true,true);
	uint32 Counter = 0;

	FString EmptyFolderPathsNames;
	TArray<FString> EmptyFoldersPathsArray;

	for (const FString& FolderPath : FolderPathsArray)
	{
		//Don't touch root folders
		if (FolderPath.Contains(TEXT("Developers")) ||
			FolderPath.Contains(TEXT("Collections")))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesDirectoryExist(FolderPath)) continue;

		if (!UEditorAssetLibrary::DoesDirectoryHaveAssets(FolderPath))
		{
			EmptyFolderPathsNames.Append(FolderPath);
			EmptyFolderPathsNames.Append(TEXT("\n"));

			EmptyFoldersPathsArray.Add(FolderPath);
		}
	}

	if (EmptyFoldersPathsArray.Num()==0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("Noempty folder found under selected folder"),false);
		return;
	}

	EAppReturnType::Type ConfirmResult = DebugHeader::ShowMsgDialog(EAppMsgType::OkCancel,
		TEXT("Empty folders found in :\n")+EmptyFolderPathsNames+TEXT("\nWould you like to deleteall?"),false);

	if (ConfirmResult == EAppReturnType::Cancel) return;

	for (const FString& EmptyFolderPath:EmptyFoldersPathsArray)
	{
		UEditorAssetLibrary::DeleteDirectory(EmptyFolderPath)?
			++Counter : DebugHeader::Print(TEXT("Failed to delete "+ EmptyFolderPath) , FColor::Red);
	}

	if (Counter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfullydeleted ") + FString::FromInt(Counter) + TEXT(" folders."));
	}

	
}

void FSuperManagerModule::OnDeleteEmptyFoldersAndUnusedAssetsButtonClicked()
{
	OnDeleteUnusedAssetButtonClicked();

	OnDeleteEmptyFoldersButtonClicked();
}

void FSuperManagerModule::OnAdvanceDeletionButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("AdvanceDeletion"));
}

void FSuperManagerModule::FixUpRedirectors()
{
	TArray<UObjectRedirector*> RedirectorsToFixArray;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.ClassNames.Emplace("ObjectRedirector");

	TArray<FAssetData> OutRedirectors;

	AssetRegistryModule.Get().GetAssets(Filter,OutRedirectors);

	for (const FAssetData& RedirectorData : OutRedirectors)
	{
		if (UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsToFixArray.Add(RedirectorToFix);
		}
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
}

#pragma endregion

#pragma region CustomEditorTab

void FSuperManagerModule::RegisterAdvanceDeletionTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName("AdvanceDeletion"),
		FOnSpawnTab::CreateRaw(this,&FSuperManagerModule::OnSpawnAdvanceDeletionTab)).SetDisplayName(FText::FromString(TEXT("Advance Deletion")));
}

TSharedRef<SDockTab> FSuperManagerModule::OnSpawnAdvanceDeletionTab(const FSpawnTabArgs& aaa)
{
	return SNew(SDockTab).TabRole(ETabRole::NomadTab);
}

#pragma endregion

void FSuperManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSuperManagerModule, SuperManager)