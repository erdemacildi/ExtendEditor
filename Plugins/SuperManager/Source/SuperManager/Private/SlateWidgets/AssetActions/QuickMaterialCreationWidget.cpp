// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AssetActions/QuickMaterialCreationWidget.h"
#include "DebugHeader.h"

#pragma region QuickMaterialCreationCore

void UQuickMaterialCreationWidget::CreateMaterialFromSelectedTextures()
{
	if (bCustomMaterialName)
	{
		if (MaterialName.IsEmpty() || MaterialName.Equals("M_"))
		{
			DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("Please enter valid name"));
			return;
		}
	}
	DebugHeader::Print(TEXT("Working"),FColor::Cyan);
}

#pragma endregion 
