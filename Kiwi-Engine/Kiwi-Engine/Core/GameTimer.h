#ifndef _KIWI_GAMETIMER_H_
#define _KIWI_GAMETIMER_H_

#include "HighResolutionTimer.h"

namespace Kiwi
{

	class GameTimer: 
		public Kiwi::HighResolutionTimer
	{
	private:

		//stores the current FPS
		int m_framesPerSecond;

		/*the current number of fixed updates each second
		this is equal to targetUpdatesPerSecond unless the current
		framerate is lower than that, in which case this value equals the frame rate*/
		int m_fixedUpdatesPerSecond;

		/*stores the desired number of fixed updates per second
		if the current FPS is lower than this value then m_fixedDeltaTime = m_deltaTime
		otherwise m_fixedDeltaTime = 1/m_targetUpdatesPerSecond*/
		int m_targetUpdatesPerSecond;

		/*optimization to avoid a divide every frame
		stores 1/m_targetUpdatesPerSecond*/
		float m_oneOverTUPS;

		/*equal to 1/FPS used to time motion and movements
		e.g. to move 1 unit per second you would increment by 1*deltaTime
		each update cycle*/
		float m_deltaTime;

		/*equal to 1/fixedUpdatesPerSecond
		this is for use in FixedUpdate() calls to do motion, instead of deltaTime*/
		float m_fixedDeltaTime;

		//true when it's time to do a fixedupdate call
		bool m_doFixedUpdate;

	public:

		GameTimer();
		~GameTimer();

		virtual void Update();

		void SetTargetUpdatesPerSecond(int updatesPerSecond);

		int GetFramesPerSecond()const { return m_framesPerSecond; }
		int GetFixedUpdatesPerSecond()const { return m_fixedUpdatesPerSecond; }
		int GetTargetUpdatesPerSecond()const { return m_targetUpdatesPerSecond; }

		float GetDeltaTime()const { return m_deltaTime; }
		float GetFixedDeltaTime()const;

		/*Call on every update cycle to determine if a FixedUpdate should occur
		this should be called only after the Update method is called*/
		bool QueryFixedUpdate();

	};
};

#endif