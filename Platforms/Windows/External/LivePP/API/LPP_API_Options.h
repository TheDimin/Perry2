// Copyright 2011-2023 Molecular Matters GmbH, all rights reserved.

#pragma once


// ------------------------------------------------------------------------------------------------
// API STATUS & OPTIONS
// ------------------------------------------------------------------------------------------------

LPP_NAMESPACE_BEGIN

typedef enum LppConnectionStatus
{
	LPP_CONNECTION_STATUS_SUCCESS,						// connecting the Agent, Bridge and Broker was successful
	LPP_CONNECTION_STATUS_FAILURE,						// general failure, check the logs for details
	LPP_CONNECTION_STATUS_UNEXPECTED_VERSION_BLOB,		// Agent and Broker are incompatible since they use different blob types
	LPP_CONNECTION_STATUS_VERSION_MISMATCH				// Agent and Broker are incompatible since their API versions do not match
} LppConnectionStatus;

typedef enum LppModulesOption
{
	LPP_MODULES_OPTION_NONE,
	LPP_MODULES_OPTION_ALL_IMPORT_MODULES
} LppModulesOption;

typedef enum LppReloadBehaviour
{
	LPP_RELOAD_BEHAVIOUR_CONTINUE_EXECUTION,
	LPP_RELOAD_BEHAVIOUR_WAIT_UNTIL_CHANGES_ARE_APPLIED
} LppReloadBehaviour;

typedef enum LppRestartOption
{
	LPP_RESTART_OPTION_CURRENT_PROCESS,
	LPP_RESTART_OPTION_ALL_PROCESSES
} LppRestartOption;

typedef enum LppRestartBehaviour
{
	LPP_RESTART_BEHAVIOUR_DEFAULT_EXIT,					// ExitProcess()
	LPP_RESTART_BEHAVIOUR_EXIT_WITH_FLUSH,				// exit()
	LPP_RESTART_BEHAVIOUR_EXIT_WITHOUT_FLUSH,			// _Exit()
	LPP_RESTART_BEHAVIOUR_INSTANT_TERMINATION			// TerminateProcess
} LppRestartBehaviour;

LPP_NAMESPACE_END
