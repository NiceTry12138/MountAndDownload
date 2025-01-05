// Copyright Epic Games, Inc. All Rights Reserved.

#include "MountPakEditor.h"
#include "Slate/SGeneratePak.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "FMMountPakEditorModule"

static const FName MountPakEditorTabName("MountPakEditor");

void FMountPakEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMountPakEditorModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MountPakEditorTabName, FOnSpawnTab::CreateRaw(this, &FMountPakEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("MountPakEditorTitle", "MountPak Editor"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FMountPakEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MountPakEditorTabName);
}

void FMountPakEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	{
		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
		{
			FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(
				"Asset Reference",
				FUIAction(FExecuteAction::CreateRaw(this, &FMountPakEditorModule::PluginButtonClicked)),
				TAttribute<FText>(),
				TAttribute<FText>(),
				FSlateIcon()
			));
			// Entry.SetCommandList(PluginCommands);
		}
	}
}

void FMountPakEditorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(MountPakEditorTabName);
}

TSharedRef<SDockTab> FMountPakEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SGeneratePak)
		];
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMountPakEditorModule, MountPakEditor)