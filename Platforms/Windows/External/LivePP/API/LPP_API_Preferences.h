// Copyright 2011-2023 Molecular Matters GmbH, all rights reserved.

#pragma once


// ------------------------------------------------------------------------------------------------
// PREFERENCES TYPES
// ------------------------------------------------------------------------------------------------

LPP_NAMESPACE_BEGIN

typedef struct LppProjectPreferences
{
	struct HotReload
	{
		const char* objectFileExtensions;
		const char* libraryFileExtensions;
		int captureToolchainEnvironmentTimeout;

		struct PreBuild
		{
			bool isEnabled;
			const wchar_t* executable;
			const wchar_t* workingDirectory;
			const char* commandLineOptions;
		} preBuild;

		bool callCompileHooksForHaltedProcesses;
		bool callLinkHooksForHaltedProcesses;
		bool callHotReloadHooksForHaltedProcesses;
	} hotReload;

	struct Compiler
	{
		const wchar_t* overrideLocation;
		const char* commandLineOptions;
		bool captureEnvironment;
		bool isOverridden;
		bool useOverrideAsFallback;
		bool forcePrecompiledHeaderPDBs;
		bool removeShowIncludes;
	} compiler;

	struct Linker
	{
		const wchar_t* overrideLocation;
		const char* commandLineOptions;
		bool captureEnvironment;
		bool isOverridden;
		bool useOverrideAsFallback;
		bool suppressCreationOfImportLibrary;
	} linker;

	struct ExceptionHandler
	{
		bool isEnabled;
		int order;					// 0 = last, 1 = first
	} exceptionHandler;

	struct ContinuousCompilation
	{
		const wchar_t* directory;
		int timeout;
		bool isEnabled;
	} continuousCompilation;

	struct VirtualDrive
	{
		const char* letterPlusColon;
		const wchar_t* directory;
		bool isEnabled;
	} virtualDrive;

	struct UnitySplitting
	{
		const char* fileExtensions;
		int threshold;
		bool isEnabled;
	} unitySplitting;
} LppProjectPreferences;

LPP_NAMESPACE_END


// ------------------------------------------------------------------------------------------------
// PREFERENCES ENUMS
// ------------------------------------------------------------------------------------------------

LPP_NAMESPACE_BEGIN

typedef enum LppBoolPreferences
{
	LPP_BOOL_PREF_LOGGING_PRINT_TIMESTAMPS,						// print timestamps in UI log
	LPP_BOOL_PREF_LOGGING_ENABLE_WORDWRAP,						// enable word wrap in UI log
	LPP_BOOL_PREF_NOTIFICATIONS_ENABLED,						// enable notifications
	LPP_BOOL_PREF_NOTIFICATIONS_PLAY_SOUND_ON_SUCCESS,			// play sound on success
	LPP_BOOL_PREF_NOTIFICATIONS_PLAY_SOUND_ON_ERROR,			// play sound on error
	LPP_BOOL_PREF_HOT_RELOAD_LOAD_INCOMPLETE_MODULES,			// load incomplete modules
	LPP_BOOL_PREF_HOT_RELOAD_LOAD_INCOMPLETE_COMPILANDS,		// load incomplete compilands
	LPP_BOOL_PREF_HOT_RELOAD_DELETE_PATCH_FILES,				// delete patch files upon process exit
	LPP_BOOL_PREF_HOT_RELOAD_CLEAR_LOG,							// clear log upon hot-reload
	LPP_BOOL_PREF_VISUAL_STUDIO_SHOW_MODAL_DIALOG				// show modal dialog in Visual Studio
} LppBoolPreferences;

typedef enum LppIntPreferences
{
	LPP_INT_PREF_HOT_RELOAD_TIMEOUT,							// timeout in milliseconds
	LPP_INT_PREF_HOT_RESTART_TIMEOUT,							// timeout in milliseconds

	LPP_INT_PREF_UI_STYLE,										// UI style:
																//	Light= 0,
																//	Dark = 1

	LPP_INT_PREF_LOGGING_VERBOSITY,								// UI verbosity:
																//	Default = 0,
																//	Detailed = 1

	LPP_INT_PREF_NOTIFICATIONS_FOCUS_TYPE,						// focus broker window upon:
																//	Never = 0,
																//	OnHotReloadOrHotRestart = 1,
																//	OnError = 2,
																//	OnSuccess = 3,
																//	Always = 4
} LppIntPreferences;

typedef enum LppStringPreferences
{
	LPP_STRING_PREF_LOGGING_FONT,								// font as formatted in the .json file, e.g. "Courier New,10,-1,2,400,0,0,0,0,0,0,0,0,0,0,1"
	LPP_STRING_PREF_NOTIFICATIONS_SOUND_ON_SUCCESS_PATH,		// absolute or relative path
	LPP_STRING_PREF_NOTIFICATIONS_SOUND_ON_ERROR_PATH			// absolute or relative path
} LppStringPreferences;

typedef enum LppShortcutPreferences
{
	LPP_SHORTCUT_PREF_HOT_RELOAD,								// shortcut for scheduling a hot-reload
	LPP_SHORTCUT_PREF_VISUAL_STUDIO_TOGGLE_OPTIMIZATIONS		// shortcut for toggling optimizations for the current file in Visual Studio
} LppShortcutPreferences;

LPP_NAMESPACE_END
