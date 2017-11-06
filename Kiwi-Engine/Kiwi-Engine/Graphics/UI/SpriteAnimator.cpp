#include "SpriteAnimator.h"
#include "IUIWidget.h"
#include "UICanvas.h"

#include "../Mesh.h"

#include "..\..\Types.h"

#include "../../Core/Entity.h"
#include "..\..\Core\EngineRoot.h"

#include <math.h>

namespace Kiwi
{

	SpriteAnimator::SpriteAnimator( const std::wstring& animatorName, Kiwi::Scene& scene, const std::wstring& textureName ):
		Kiwi::Component( animatorName, Kiwi::ComponentType::SpriteAnimator, scene )
	{
		m_textureName = textureName;
		m_loopAnimation = false;
		m_endOnFirstFrame = false;
		m_currentFrame = 0;
		m_frameTimer = 0.0;
		m_currentFrameDuration = DBL_MAX;
		m_status = STOPPED;
	}

	void SpriteAnimator::_OnUpdate()
	{
		if( m_started && m_status == PLAYING )
		{
			m_frameTimer += _kGameTimer.GetDeltaTime();

			/*keep animating until the frame timer has caught up to the current frame duration*/
			while( m_frameTimer > m_currentFrameDuration )
			{
				m_frameTimer -= m_currentFrameDuration;

				auto itr = m_animations.find( m_activeAnimation );
				if( itr != m_animations.end() )
				{
					/*get the mesh from the entity*/
					Kiwi::Mesh* entityMesh = m_entity->FindComponent<Kiwi::Mesh>( Kiwi::ComponentType::Mesh );
					if( entityMesh == nullptr )
					{
						m_frameTimer = 0.0;
						return;
					}

					/*check if the end of the animation has been reached*/
					if( m_currentFrame >= itr->second.second.size() )
					{
						if( m_loopAnimation )
						{
							m_currentFrame = 0; //if looping, just reset back to the first frame and continue

						} else
						{ /*if not looping, stop the animation*/
							m_currentFrame = ( m_endOnFirstFrame ) ? 0 : m_currentFrame - 1; //reset to the first frame if needed, otherwise stay on the last frame
							m_status = STOPPED;
							m_frameTimer = 0.0;
							return;
						}
					}

					/*find the submesh(es) whose material's diffuse texture matches the animator texture*/
					//for( uint i = 0; i < entityMesh->GetSubmeshCount(); i++ )
					//{
					//	Kiwi::Submesh* submesh = entityMesh->GetSubmesh( i );
					//	if( submesh != nullptr && submesh->material.GetTexture( Kiwi::Material::TEXTURE_TYPE::DIFFUSE ) != nullptr )
					//	{
					//		if( submesh->material.GetTexture( Kiwi::Material::TEXTURE_TYPE::DIFFUSE )->GetAssetName().compare( m_textureName ) == 0 )
					//		{
					//			/*matching submesh found, now edit the uvs in the mesh at the correct submesh position to the new frame's uvs*/
					//			std::vector<Kiwi::Vector2d> uvs = entityMesh->GetUVs();
					//			for( uint uvIndex = submesh->startIndex; uvIndex < uvs.size(), uvIndex < 6; uvIndex++ )
					//			{
					//				uvs[uvIndex] = itr->second.second[m_currentFrame][uvIndex];
					//			}
					//			/*submit the new uvs to the mesh and commit the changes*/
					//			entityMesh->SetUVs( uvs );
					//			entityMesh->Commit();
					//		}
					//	}
					//}

					Kiwi::IUIWidget* widget = dynamic_cast<Kiwi::IUIWidget*>(m_entity);
					if( widget != nullptr )
					{
						widget->GetWidgetCanvas()->UpdateWidgets();
					}

					m_currentFrame++;
				}
			}
		}
	}

	void SpriteAnimator::Play( const std::wstring& animationName, bool loopAnimation, bool endOnFirstFrame, bool interruptIfPlaying )
	{
		auto itr = m_animations.find( animationName );
		if( itr != m_animations.end() )
		{
			bool sameAnimation = animationName.compare( m_activeAnimation ) == 0;

			if( sameAnimation && m_status == PAUSED && !interruptIfPlaying )
			{
				m_status = PLAYING;

			}else if( sameAnimation == false || interruptIfPlaying )
			{
				m_activeAnimation = animationName;
				m_currentFrame = 0;
				m_currentFrameDuration = 1.0 / (double)itr->second.first;
				m_frameTimer = m_currentFrameDuration; //reset the timer to play immediately
				m_loopAnimation = loopAnimation;
				m_endOnFirstFrame = endOnFirstFrame;
				m_status = PLAYING;
			}
		}
	}

	void SpriteAnimator::Stop()
	{
		m_status = STOPPED;
		m_frameTimer = 0.0;
		m_currentFrame = 0;
		m_currentFrameDuration = DBL_MAX;
	}

	void SpriteAnimator::AddAnimation( const std::wstring& animationName, unsigned int framesPerSecond, std::vector<std::vector<Kiwi::Vector2d>> frameUVs )
	{
		auto itr = m_animations.find( animationName );
		if( itr == m_animations.end() )
		{
			m_animations.insert( std::make_pair( animationName, std::make_pair( framesPerSecond, frameUVs ) ) );
		}
	}

}