#include "GameTimer.h"
#include "Utilities.h"

namespace Kiwi
{

	GameTimer::GameTimer()
	{

		m_framesPerSecond = 1;
		m_fixedUpdatesPerSecond = 1;
		m_targetUpdatesPerSecond = 60;
		m_oneOverTUPS = 1.0 / 60.0;

		m_deltaTime = 0.0f;
		m_fixedDeltaTime = m_oneOverTUPS;

		m_doFixedUpdate = false;

		m_frameCount = 0;

	}

	GameTimer::~GameTimer()
	{

	}

	void GameTimer::Update()
	{

		if(m_started == false) return;

		static double timeSinceFixedUpdate = 0.0;
		static double elapsedTime = 0.0;
		static int lastFPSFrame = 0; //stores the frame count when the fps was last calculated
		//static double FPSTimer = 0.0;

		//update the main timer
		this->_Tick();

		m_frameCount++;

		elapsedTime += m_frameTime;
		timeSinceFixedUpdate += m_frameTime;
		m_doFixedUpdate = false;
		m_deltaTime = (float)m_frameTime;

		/*if deltaTime was less than the current fixedDeltaTime, set fixedDeltaTime to equal deltaTime*/
		if( m_fixedDeltaTime < m_deltaTime )
		{
			m_fixedDeltaTime = m_deltaTime;
			m_fixedUpdatesPerSecond = (int)(1.0 / m_frameTime);

		} else
		{
			m_fixedDeltaTime = (float)m_oneOverTUPS;
			m_fixedUpdatesPerSecond = m_targetUpdatesPerSecond;
		}

		/*if enough time has passed, it is time to do a fixed update*/
		if( timeSinceFixedUpdate >= m_fixedDeltaTime )
		{
			m_doFixedUpdate = true;
			timeSinceFixedUpdate -= m_fixedDeltaTime;
		}

		/*calculates the current FPS once per second*/
		if( elapsedTime >= 1.0 )
		{
			m_framesPerSecond = m_frameCount - lastFPSFrame;
			elapsedTime -= 1.0;
			lastFPSFrame = m_frameCount;
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

		m_oneOverTUPS = 1.0/(double)m_targetUpdatesPerSecond;

	}

};