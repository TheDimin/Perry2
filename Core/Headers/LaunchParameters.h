#pragma once

// Original source: https://github.com/BredaUniversityGames/Y2023-24-PR-Ball/blob/main/Engine/Headers/Utilities/LaunchParameters.h
// Created by: Martijn Hagenaars
// Created in: 2023 for `OnTheBubble`

namespace Perry
{
	class LaunchParameters
	{
	public:

		~LaunchParameters() = default;
		void SetParameters(const std::vector<std::string>& parameters);
		void Clear();
		int Count() const;

		bool Contains(const std::string& parameter) const;


		int GetInt(const std::string& parameter, int fallbackValue) const;
		float GetFloat(const std::string& parameter, float fallbackValue) const;
		std::string GetString(const std::string& parameter, std::string fallbackValue) const;

		std::string ToString()const;

		const std::unordered_map<std::string, std::string>& View() const { return commandMap; }
	private:


		friend const LaunchParameters& GetLaunchParameters();
		LaunchParameters() = default;

		std::unordered_map<std::string, std::string> commandMap{};
	};

	extern const LaunchParameters& GetLaunchParameters();

}