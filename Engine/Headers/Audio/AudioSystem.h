#pragma once

namespace FMOD
{
	class Sound;
	class Channel;
}

namespace FMOD
{
	namespace Studio
	{
		class EventInstance;
		class Channel;
		class Sound;
		class Bank;
		class System;
		class Bus;
	}
}
namespace Perry
{
	class TransformComponent;

	class AudioSystem : public ISubSystem
	{
	public:

		/// <summary>
		/// Creates/Gets an fmod Event instance
		/// </summary>
		/// <param name="eventName"></param>
		/// <returns></returns>
		FMOD::Studio::EventInstance* GetEvent(const std::string& eventName);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="filepath">elative to Audio resources folder</param>
		/// <param name="playMode"></param>
		/// <returns>Handle to audio </returns>
		FMOD::Sound* PlaySound(const ::std::string& filepath, FMOD_MODE playMode = FMOD_DEFAULT);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="filepath"></param>
		///<returns> Handle to bank</returns>
		FMOD::Studio::Bank* LoadBank(const std::string& filepath);

		FMOD::System* GetCoreSystem() { return m_CoreSystem; }

		FMOD::Studio::Bus* GetBus(const std::string& busName) const;
		std::vector<FMOD::Studio::Bus*> GetAllBuses() const;
		std::string GetBusPath(FMOD::Studio::Bus*) const;

		std::unordered_map<std::string, FMOD::Studio::EventInstance*> GetEvents();

		FMOD::Studio::EventInstance* StartEvent(const std::string& eventPath);
		bool IsEventPlaying(FMOD::Studio::EventInstance* eventInstance);
		void Set3DEventAttributes(FMOD::Studio::EventInstance* eventInst, const TransformComponent& transform, const glm::vec3& velocity);

		/// <summary>
		/// Allows you to Play an event multiple times at the same time without resetting it
		/// </summary>
		/// <param name="eventPath"></param>
		FMOD::Studio::EventInstance* EmitEvent(const std::string& eventPath);

		float GetVolume(FMOD::Studio::Bus* bus)const;
		bool GetMuted(FMOD::Studio::Bus* bus) const;
		void SetVolume(FMOD::Studio::Bus* bus, float volume);
		void ToggleMute(FMOD::Studio::Bus* bus, bool muted);

		bool ValidateAudioSettings(const std::string& filepath);
	protected:
		void Init() override;
		void ShutDown() override;
		void Update(float deltaTime) override;

	private:
		FMOD::System* m_CoreSystem = nullptr;
		FMOD::Studio::System* m_System = nullptr;
		std::queue<FMOD::Studio::EventInstance*> m_EmittedEvents{};

		std::unordered_map<std::string, FMOD::Studio::EventInstance*> m_Events{};
	};
} // namespace Perry