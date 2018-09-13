// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "lua_wrapper.h"
#include "lua_wrapperStyle.h"
#include "lua_wrapperCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "FileManager.h"

#ifdef _MSC_VER
#include "windows.h"
#endif

static const FName lua_wrapperTabName("LuaWrapper");

#define LOCTEXT_NAMESPACE "Flua_wrapperModule"

void Flua_wrapperModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	Flua_wrapperStyle::Initialize();
	Flua_wrapperStyle::ReloadTextures();

	Flua_wrapperCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		Flua_wrapperCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &Flua_wrapperModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &Flua_wrapperModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &Flua_wrapperModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(lua_wrapperTabName, FOnSpawnTab::CreateRaw(this, &Flua_wrapperModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("Flua_wrapperTabTitle", "lua_wrapper"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void Flua_wrapperModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	Flua_wrapperStyle::Shutdown();

	Flua_wrapperCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(lua_wrapperTabName);
}

TSharedRef<SDockTab> Flua_wrapperModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("Flua_wrapperModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("lua_wrapper.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()[
					SNew(STextBlock)
					.Text(WidgetText)
				]
				+ SVerticalBox::Slot()[
					SNew(STextBlock)
					.Text(WidgetText)
				]
				+ SVerticalBox::Slot()[
					SNew(SEditableTextBox)
					.Text(WidgetText)
				]
			]
		];
}

void Flua_wrapperModule::PluginButtonClicked()
{
	//FGlobalTabmanager::Get()->InvokeTab(lua_wrapperTabName);
#ifdef _MSC_VER
	FString RelativePath = FPaths::GameContentDir();
	FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);
	auto wrapperExe = FullPath + TEXT("/../Tools/lua-wrapper.exe");
	system(TCHAR_TO_UTF8(*wrapperExe));
#else
	
#endif
}

void Flua_wrapperModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(Flua_wrapperCommands::Get().OpenPluginWindow);
}

void Flua_wrapperModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(Flua_wrapperCommands::Get().OpenPluginWindow);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(Flua_wrapperModule, lua_wrapper)