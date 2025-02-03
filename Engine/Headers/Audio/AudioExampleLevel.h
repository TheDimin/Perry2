#pragma once

namespace FMOD
{
	class Sound;
	namespace Studio {
		class Bank;
	}
}  // namespace FMOD

namespace Perry
{
	class AudioExampleLevel : public Level {
	public:
		void OnLoad() override;
		void OnUnload() override;

	private:
		FMOD::Studio::Bank* masterBank = nullptr;
		FMOD::Studio::Bank* stringBank = nullptr;
		FMOD::Studio::Bank* SFXBank = nullptr;
		FMOD::Sound* audioEffect = nullptr;
	};
}  // namespace Perry