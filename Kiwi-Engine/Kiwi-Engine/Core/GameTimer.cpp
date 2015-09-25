#include "GameTimer.h"
#include "Logger.h"
#include "Utilities.h"

namespace Kiwi
{

	GameTimer::GameTimer()
	{

		m_framesPerSecond = 1;
		m_fixedUpdatesPerSecond = 1;
		m_targetUpdatesPerSecond = 60;
		m_oneOverTUPS = 1.0f / 60.0f;

		m_deltaTime = 0.0f;
		m_fixedDeltaTime = 0.0f;

		m_doFixedUpdate = false;

	}

	GameTimer::~GameTimer()
	{

	}

	void GameTimer::Update()
	{

		if(m_started == false) return;

		static float elapsedTime = 0.0f;
		static int frameCounter = 0;
		static float FPSTimer = 0.0f;

		frameCounter++;

		//update the main timer
		this->_Tick();

		//if enough time has passed, time to do a FixedUpdate
		if(m_frameTime > 1.0)
		{
			elapsedTime += 1.0f;
			FPSTimer += 1.0f;
		}else
		{
			elapsedTime += (float)m_frameTime;
			FPSTimer += (float)m_frameTime;
		}
		if(elapsedTime >= m_fixedDeltaTime)
		{
			m_doFixedUpdate = true;
			elapsedTime -= m_fixedDeltaTime;
		}

		if(FPSTimer >= 1.0f)
		{
			m_framesPerSecond = frameCounter;
			FPSTimer -= 1.0f;
			frameCounter = 0;
		}

		m_deltaTime = (float)m_frameTime;//1.0f/(float)m_framesPerSecond;

		if(m_fixedDeltaTime < m_deltaTime)
		{
			m_fixedDeltaTime = m_deltaTime;
			m_fixedUpdatesPerSecond = m_framesPerSecond;
		}else
		{
			m_fixedDeltaTime = m_oneOverTUPS;
			m_fixedUpdatesPerSecond = m_targetUpdatesPerSecond;
		}
	}

	void GameTimer::SetTargetUpdatesPerSecond(int updatesPerSecond)
	{

		if(updatesPerSecond < 1)
		{
			m_targetUpdatesPerSecond = 1;
		}else
		{
			m_targetUpdatesPerSecond = updatesPerSecond;
		}

		m_oneOverTUPS = 1.0f/(float)m_targetUpdatesPerSecond;
		m_fixedDeltaTime = m_oneOverTUPS;

	}

	float GameTimer::GetFixedDeltaTime()const
	{

		return m_fixedDeltaTime;

	}

	bool GameTimer::QueryFixedUpdate()
	{

		if(m_doFixedUpdate == true)
		{
			m_doFixedUpdate = false;
			return true;
		}

		return false;
	}

};