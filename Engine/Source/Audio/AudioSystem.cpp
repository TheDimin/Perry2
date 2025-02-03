#include "EnginePCH.h"

using namespace Perry;

static void ASSERTION(FMOD_RESULT r)
{
	if ((r) != FMOD_OK) { printf("FMOD error! (%d) %s\n", r, FMOD_ErrorString((r))); assert(false); }
}

#define ERRCHECK(state) ASSERTION(state)


void AudioSystem::Init()
{
	ERRCHECK(FMOD::Studio::System::create(&m_System));

	// The example Studio project is authored for 5.1 sound, so set up the system output mode to match
	ERRCHECK(m_System->getCoreSystem(&m_CoreSystem));
	ERRCHECK(m_CoreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_7POINT1POINT4, 0));


	FMOD_STUDIO_INITFLAGS studioFlags = FMOD_STUDIO_INIT_NORMAL;
#if !defined(_SHIPPING) && defined(PLATFORM_WINDOWS)
	studioFlags |= FMOD_STUDIO_INIT_LIVEUPDATE;
#endif

	ERRCHECK(m_System->initialize(1024, studioFlags, FMOD_INIT_NORMAL, nullptr));
}

void AudioSystem::ShutDown()
{
	m_System->release();
	m_CoreSystem->release();

	for (auto& eventPair : m_Events)
	{
		eventPair.second->release();
	}
	m_Events.clear();
	if (!m_EmittedEvents.empty())
		for (auto i = 0; i < m_EmittedEvents.size(); i++)
		{
			m_EmittedEvents.front()->release();
			m_EmittedEvents.pop();
		}
}

void AudioSystem::Update(float deltaTime)
{
	m_System->update();

	if (!m_EmittedEvents.empty())
	{
		auto emittedEvent = m_EmittedEvents.front();

		FMOD_STUDIO_PLAYBACK_STATE sfxState;
		emittedEvent->getPlaybackState(&sfxState);
		if (sfxState != FMOD_STUDIO_PLAYBACK_PLAYING)
		{
			emittedEvent->release();
			m_EmittedEvents.pop();
		}
	}
}

FMOD::Studio::EventInstance* AudioSystem::GetEvent(const std::string& eventName)
{
	if (m_Events.find(eventName) == m_Events.end())
	{
		FMOD::Studio::EventDescription* eventDescription = NULL;
		ERRCHECK(m_System->getEvent(eventName.c_str(), &eventDescription));

		FMOD::Studio::EventInstance* eventInstance = NULL;
		ERRCHECK(eventDescription->createInstance(&eventInstance));

		m_Events.emplace(eventName, eventInstance);
		return eventInstance;
	}
	else
	{
		return m_Events[eventName];
	}
}

FMOD::Sound* AudioSystem::PlaySound(const ::std::string& filepath, FMOD_MODE playMode)
{
	FMOD::System* coreSystem = NULL;
	ERRCHECK(m_System->getCoreSystem(&coreSystem));

	FMOD::Sound* mysound = nullptr;
	::std::string lll = (FileIO::GetPath(DirectoryType::Resource) + "Audio/" + filepath);

	ERRCHECK(coreSystem->createSound(lll.c_str(), FMOD_2D, 0, &mysound));

	ERRCHECK(mysound->setMode(playMode));
	FMOD::Channel* channel1 = nullptr;

	coreSystem->getChannel(0, &channel1);

	ERRCHECK(coreSystem->playSound(mysound, 0, false, &channel1));

	return mysound;
}

FMOD::Studio::Bank* AudioSystem::LoadBank(const std::string& filepath)
{
	::std::string lll = FileIO::GetPath(DirectoryType::Resource, "Audio/" + filepath);
	printf("%s \n", lll.c_str());
	FMOD::Studio::Bank* bank = nullptr;
	ERRCHECK(m_System->loadBankFile(lll.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank));
	return bank;
}

FMOD::Studio::Bus* Perry::AudioSystem::GetBus(const std::string& busName) const
{
	int bankCount = 0;
	ERRCHECK(m_System->getBankCount(&bankCount));

	if (bankCount == 0)
		return NULL;

	FMOD::Studio::Bus* audioBus = NULL;
	ERRCHECK(m_System->getBus(busName.c_str(), &audioBus));
	return audioBus;
}

std::vector<FMOD::Studio::Bus*> Perry::AudioSystem::GetAllBuses() const
{
	int count = 0;
	m_System->getBankCount(&count);

	if (count == 0)
		return std::vector<FMOD::Studio::Bus*>{};

	FMOD::Studio::Bank* master = NULL;
	ERRCHECK(m_System->getBank("bank:/Master", &master));

	int busCount = 0;
	ERRCHECK(master->getBusCount(&busCount));

	std::vector<FMOD::Studio::Bus*> buses;
	buses.resize(busCount);
	ERRCHECK(master->getBusList(buses.data(), (int)buses.size(), &busCount));

	return buses;
}

std::string Perry::AudioSystem::GetBusPath(FMOD::Studio::Bus* bus) const
{
	if (bus == NULL)
		return nullptr;

	char busName[16];
	int retrieved = 0;
	ERRCHECK(bus->getPath(busName, 16, &retrieved));

	return busName;
}

std::unordered_map<std::string, FMOD::Studio::EventInstance*> AudioSystem::GetEvents()
{
	return m_Events;
}

FMOD::Studio::EventInstance* Perry::AudioSystem::StartEvent(const std::string& eventPath)
{
	auto* eventInstance = GetAudio().GetEvent(eventPath);
	FMOD_STUDIO_PLAYBACK_STATE sfxState;
	eventInstance->getPlaybackState(&sfxState);
	if (sfxState != FMOD_STUDIO_PLAYBACK_PLAYING)
	{
		eventInstance->start();
	}

	return eventInstance;
}

bool Perry::AudioSystem::IsEventPlaying(FMOD::Studio::EventInstance* eventInstance)
{
	FMOD_STUDIO_PLAYBACK_STATE sfxState;
	eventInstance->getPlaybackState(&sfxState);
	if (sfxState == FMOD_STUDIO_PLAYBACK_PLAYING)
	{
		return true;
	}
	return false;
}

void Perry::AudioSystem::Set3DEventAttributes(FMOD::Studio::EventInstance* eventInst, const TransformComponent& transform, const glm::vec3& velocity)
{
	const auto& pos = Transform::GetPosition(transform);

	FMOD_VECTOR fmodPos = { pos.x, pos.y, pos.z };
	FMOD_VECTOR fmodVelocity = { velocity.x, velocity.y, velocity.z };
	FMOD_VECTOR fmodForward = { 0.f, 0.f, -1.f };
	FMOD_VECTOR fmodUp = { 0.f, 1.f, 0.f };

	FMOD_3D_ATTRIBUTES attribs =
	{
		fmodPos,
		fmodVelocity,
		fmodForward,
		fmodUp,
	};

	eventInst->set3DAttributes(&attribs);
}

FMOD::Studio::EventInstance* Perry::AudioSystem::EmitEvent(const std::string& eventPath)
{
	FMOD::Studio::EventDescription* eventDescription = NULL;
	ERRCHECK(m_System->getEvent(eventPath.c_str(), &eventDescription));

	FMOD::Studio::EventInstance* eventInstance = NULL;
	ERRCHECK(eventDescription->createInstance(&eventInstance));
	eventInstance->start();
	m_EmittedEvents.push(eventInstance);

	return eventInstance;
}

float AudioSystem::GetVolume(FMOD::Studio::Bus* bus) const
{
	if (bus == NULL)
		return -1;

	float value;
	bus->getVolume(&value);
	return value;
}

bool Perry::AudioSystem::GetMuted(FMOD::Studio::Bus* bus) const
{
	if (bus == NULL)
		return false;

	bool value;
	bus->getMute(&value);
	return value;
}

void AudioSystem::SetVolume(FMOD::Studio::Bus* bus, float volume)
{
	if (bus == NULL)
		return;

	bus->setVolume(volume);
	m_System->update();
}

void AudioSystem::ToggleMute(FMOD::Studio::Bus* bus, bool muted)
{
	if (bus == NULL)
		return;

	bus->setMute(muted);
	m_System->update();
}

bool Perry::AudioSystem::ValidateAudioSettings(const std::string& filepath)
{
	ASSERT_MSG(LOG_AUDIO, FileIO::Exist(DirectoryType::Resource, filepath), "File [%s] not found to serialize data from!\n", filepath.c_str());

	std::string data = FileIO::Read(DirectoryType::Resource, filepath);
	nlohmann::json j = nlohmann::json::parse(data);

	nlohmann::json::array_t buses = j["BusSettings"];

	// if (buses.size() == GetEngine().GetImguiSettings().audioSettings.size())
	// {
	// 	for (auto& bus : buses)
	// 	{
	// 		if (GetEngine().GetImguiSettings().audioSettings.find(bus["Path"]) == GetEngine().GetImguiSettings().audioSettings.end())
	// 			return false;
	// 	}
	// 	return true;
	// }

	return false;
}
