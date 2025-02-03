#pragma once

namespace Perry
{
	class World;
	class GameBase : public Perry::ISubSystem
	{
	public:
		virtual ~GameBase() = default;

		virtual void Init();
		virtual void Update(float DeltaTime);
		virtual void ShutDown();

		bool IsGamePaused()const { return Paused; }
		void IsGamePaused(bool IsPaused) { Paused = IsPaused; }
		float GetPlayTime() const { return ElapsedSeconds; }

	private:
		float ElapsedSeconds = 0;
		bool Paused = false;
	};
} // namespace Perry