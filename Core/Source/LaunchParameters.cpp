#include "Core.h"
#pragma hdrstop

using namespace Perry;

void LaunchParameters::SetParameters(const std::vector<std::string>& parameters)
{
	for (size_t i = 0; i < parameters.size(); i++)
	{
		std::string parameter = parameters[i];
		if (commandMap.find(parameter) == commandMap.end() && parameter.at(0) == '-')
		{
			size_t valueDefinitionIndex = parameter.find_first_of('=');
			std::string command = parameter.substr(1, valueDefinitionIndex - 1);
			if (valueDefinitionIndex != std::string::npos)
			{
				std::string value = parameter.substr(valueDefinitionIndex + 1, parameter.length());
				commandMap.emplace(command, value);
			}
			else
			{
				commandMap.emplace(command, "");
			}
		}
		else
			ASSERT_MSG(LOG_LAUNCHPARAM,
				"Failed to parse parameter %s: parameter either already exists or is not prefixed with '-'.",
				parameter.c_str());
	}
}

void LaunchParameters::Clear()
{
	commandMap.clear();
}

int LaunchParameters::Count() const
{
	return static_cast<int>(commandMap.size());
}

bool LaunchParameters::Contains(const std::string& parameter) const
{
	return commandMap.find(parameter) != commandMap.end();
}

int LaunchParameters::GetInt(const std::string& parameter, int fallbackValue) const
{
	auto data = commandMap.find(parameter);
	if (data != commandMap.end())
	{
		// Check if we can convert to an integer
		char* endCharPtr = nullptr;
		std::strtol(data->second.c_str(), &endCharPtr, 10);
		if (*endCharPtr == '\0')
		{
			return std::stoi(data->second);
		}
		else
		{
			WARN(LOG_LAUNCHPARAM,
				"Called GetInt() for a parameter that doesn't hold an integer value: %s -> %s",
				parameter.c_str(),
				data->second.c_str());
			return fallbackValue;
		}
	}
	else
	{
		INFO(LOG_LAUNCHPARAM, "Launch parameter %s not found", parameter.c_str());
		return fallbackValue;
	}
}

float LaunchParameters::GetFloat(const std::string& parameter, float fallbackValue) const
{
	auto data = commandMap.find(parameter);
	if (data != commandMap.end())
	{
		// Check if we can convert to a float
		char* endCharPtr = nullptr;
		std::strtod(data->second.c_str(), &endCharPtr);
		if (*endCharPtr == '\0')
		{
			return std::stof(data->second);
		}
		else
		{
			WARN(LOG_LAUNCHPARAM,
				"Called GetFloat() for a parameter that doesn't hold a float value: %s -> %s",
				parameter.c_str(),
				data->second.c_str());
			return fallbackValue;
		}
	}
	else
	{
		INFO(LOG_LAUNCHPARAM, "Launch parameter %s not found", parameter.c_str());
		return fallbackValue;
	}
}

std::string LaunchParameters::GetString(const std::string& parameter, std::string fallbackValue) const
{
	auto data = commandMap.find(parameter);
	if (data != commandMap.end())
	{
		// Assert when value for launch paramater is empty
		if (data->second.empty())
			WARN(LOG_LAUNCHPARAM,
				"Called GetString() for a parameter that doesn't hold a string value: %s",
				parameter.c_str());

		return data->second;
	}

	INFO(LOG_LAUNCHPARAM, "Launch parameter %s not found", parameter.c_str());
	return fallbackValue;
}

std::string LaunchParameters::ToString() const
{
	std::string result = "";

	for (auto& value : commandMap)
	{
		result += "-" + value.first;

		if (!value.second.empty())
			result += "=" + value.second + " ";

		result += "\n";
	}

	return result;
}

const LaunchParameters& Perry::GetLaunchParameters()
{
	
	static LaunchParameters launchParameters = LaunchParameters();
	return launchParameters;
}