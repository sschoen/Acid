#include "Engine.hpp"

#include "Maths/Maths.hpp"

namespace fl
{
	Engine *Engine::INSTANCE = nullptr;

	Engine::Engine(const bool &emptyRegister) :
		m_start(HighResolutionClock::now()),
		m_timeOffset(0.0f),
		m_moduleRegister(ModuleRegister()),
		m_fpsLimit(-1.0f),
		m_initialized(false),
		m_running(true),
		m_error(false)
	{
		INSTANCE = this;

		if (!emptyRegister)
		{
			m_moduleRegister.FillRegister();
		}
	}

	Engine::~Engine()
	{
	}

	int Engine::Run() const
	{
		try
		{
			while (m_running)
			{
				m_updater->Update(m_moduleRegister);
			}

			return EXIT_SUCCESS;
		}
		catch (const std::runtime_error &e)
		{
			fprintf(stderr, "Flounder has hit an exception!\n");
			fprintf(stderr, "%s\n", e.what());
			return EXIT_FAILURE;
		}
	}

	void Engine::RequestClose(const bool &error)
	{
		m_running = false;

		// A statement in case it was already true.
		if (error)
		{
			m_error = true;
		}
	}

	float Engine::GetTimeMs() const
	{
		return std::chrono::duration_cast<MillisecondsType>(HighResolutionClock::now() - m_start).count() + (m_timeOffset / 1000.0f);
	}

	std::string Engine::GetDateTime()
	{
		time_t rawtime;
		struct tm *timeinfo;
		char buffer[80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, sizeof(buffer), "%Y-%m-%d-%I%M%S", timeinfo);
		std::string str = std::string(buffer);
		return str;
	}
}
