#ifndef _KIWI_SPRITEANIMATOR_H_
#define _KIWI_SPRITEANIMATOR_H_

#include "../../Core/Vector2d.h"
#include "..\..\Core\Component.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Kiwi
{
	
	class Texture;
	class Scene;

	class SpriteAnimator:
		public Kiwi::Component
	{
	protected:

		enum STATUS { PLAYING, PAUSED, STOPPED };

		std::wstring m_textureName;

		STATUS m_status;

		/*stores the animations in this animation set
		the key is the name of the animation, the value is a pair containing, first, the frame rate and, second, a vector of vectors where each inner vector contains the texture UV coordinates for 
		one frame of the animation and the number of these inner vectors corresponds to the number of frames in the animation*/
		std::unordered_map<std::wstring, std::pair<unsigned int, std::vector<std::vector<Kiwi::Vector2d>>>> m_animations;

		std::wstring m_activeAnimation;

		double m_frameTimer;
		double m_currentFrameDuration; //duration of each frame of animation

		unsigned int m_currentFrame;

		bool m_loopAnimation;
		bool m_endOnFirstFrame; //if true, when an animation ends it will loop around and stop on the first frame

	protected:

		void _OnUpdate();

	public:

		SpriteAnimator( const std::wstring& animatorName, Kiwi::Scene& scene, const std::wstring& textureName );

		/*starts a new animation*/
		void Play( const std::wstring& animationName, bool loopAnimation = false, bool endOnFirstFrame = false, bool interruptIfPlaying = false );

		void Pause() { m_status = PAUSED; }
		void Stop();

		void AddAnimation( const std::wstring& animationName, unsigned int framesPerSecond, std::vector<std::vector<Kiwi::Vector2d>> frameUVs );

	};
}

#endif