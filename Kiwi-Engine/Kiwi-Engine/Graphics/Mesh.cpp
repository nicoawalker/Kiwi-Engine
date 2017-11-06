#include "Mesh.h"
#include "IMeshAsset.h"
//#include "StaticMeshAsset.h"
#include "GPUBuffer.h"
#include "Renderer.h"
#include "IModel.h"

#include "..\Types.h"

#include "../Core/Entity.h"
#include "..\Core\Scene.h"
#include "../Core/Utilities.h"
#include "..\Core\Exception.h"
#include "../Core/EngineRoot.h"

namespace Kiwi
{

	Mesh::Mesh( Kiwi::Scene& scene, const std::wstring& name ) :
		Kiwi::Component( name, Kiwi::ComponentType::Mesh, scene )
	{
		m_meshModel = nullptr;
		m_copyModelOnFetch = false;
		m_isShared = false;

		m_renderTarget = L"Backbuffer";
		m_renderGroup = L"";
		m_renderGroupID = 0;
		m_viewportIndex = 0;
		m_renderTargetID = 0;
		m_addedToRenderTargetQueue = false;

		this->_SetRenderTarget( L"Backbuffer" );
	}

	Mesh::Mesh( Kiwi::Scene& scene, const std::wstring& name, const std::wstring& model ) :
		Kiwi::Component( name, Kiwi::ComponentType::Mesh, scene )
	{
		m_meshModel = nullptr;
		m_copyModelOnFetch = false;
		m_isShared = false;
		m_modelName = model;

		m_renderTarget = L"Backbuffer";
		m_renderGroup = L"";
		m_renderGroupID = 0;
		m_viewportIndex = 0;
		m_renderTargetID = 0;
		m_addedToRenderTargetQueue = false;

		this->_SetRenderTarget( L"Backbuffer" );
	}

	Mesh::~Mesh()
	{
		this->_UnsetRenderTarget();
	}

	bool Mesh::_AddToRenderTargetQueue()
	{
		if( m_meshModel == nullptr ) return false;

		/*get a pointer to the render target*/
		Kiwi::Renderer* renderer = _kGraphics.GetActiveRenderer();
		if( renderer == nullptr ) return false;

		Kiwi::RenderTarget* rt = renderer->FindRenderTarget( m_renderTarget );
		if( rt == nullptr )
		{
			m_renderTarget = L"";
			m_renderTargetID = 0;
			m_renderGroupID = 0;
			m_viewportIndex = 0;
			m_addedToRenderTargetQueue = false;
			return false;
		}

		/*add the mesh to the render target's render queue*/
		Kiwi::RenderQueue* rq = &rt->GetRenderQueue();
		rq->AddMesh( *this );

		m_addedToRenderTargetQueue = true;

		return true;
	}

	Kiwi::EventResponse Mesh::_OnAssetFetched( Kiwi::IAsset* asset )
	{
		/*take no action if the asset isn't right*/
		if( asset == nullptr || asset->GetAssetName().compare(m_modelName) != 0 ) return Kiwi::EventResponse::NONE;

		/*attempt to link the model*/
		this->SetModel( dynamic_cast<Kiwi::IModel*>(asset), m_copyModelOnFetch );

		/*stop listening if the model was successfully linked*/
		return ( m_meshModel != nullptr ) ? Kiwi::EventResponse::DISCONNECT_THIS : Kiwi::EventResponse::NONE;
	}

	void Mesh::_OnAttached()
	{
		m_objectName = m_entity->GetName() + L"/Mesh";
	}

	void Mesh::_OnActivate()
	{
		this->_AddToRenderTargetQueue();
	}

	void Mesh::_OnDeactivate()
	{
		this->_RemoveFromRenderTargetQueue();
	}

	void Mesh::_OnShutdown()
	{
		if( m_meshModel != nullptr )
		{
			m_meshModel->_UnlinkMesh( *this );
			m_meshModel->Free();
		}

		this->_RemoveFromRenderTargetQueue();
	}

	void Mesh::_OnStart()
	{
		/*if a mesh asset was specified before the mesh was started, attempt to retreive it now*/
		if( m_meshModel == nullptr && m_modelName.size() > 0 )
		{
			this->FetchAsset( L"Model", m_modelName );
		}

		this->_AddToRenderTargetQueue();
	}

	void Mesh::_RemoveFromRenderTargetQueue()
	{
		/*get a pointer to the render target*/
		Kiwi::Renderer* renderer = _kGraphics.GetActiveRenderer();
		if( renderer == nullptr ) return;

		Kiwi::RenderTarget* rt = renderer->FindRenderTarget( m_renderTarget );
		if( rt == nullptr )
		{
			m_renderTarget = L"";
			m_renderTargetID = 0;
			m_renderGroupID = 0;
			m_viewportIndex = 0;
			m_addedToRenderTargetQueue = false;
			return;
		}

		/*add the mesh to the render target's render queue*/
		Kiwi::RenderQueue* rq = &rt->GetRenderQueue();
		rq->RemoveMesh( *this );

		m_addedToRenderTargetQueue = false;
	}

	bool Mesh::_SetRenderTarget( const std::wstring& renderTarget )
	{
		/*get a pointer to the render target*/
		Kiwi::Renderer* renderer = _kGraphics.GetActiveRenderer();
		if( renderer == nullptr ) return false;

		Kiwi::RenderTarget* rt = renderer->FindRenderTarget( renderTarget );
		if( rt == nullptr )
		{
			m_renderTarget = L"";
			m_renderTargetID = 0;
			m_renderGroupID = 0;
			m_viewportIndex = 0;
			return false;
		}

		/*update the mesh's stored render target and render target id*/
		m_renderTarget = renderTarget;
		m_renderTargetID = rt->GetUID();

		/*add the mesh to the render target's render queue*/
		if( m_meshModel != nullptr )
		{
			Kiwi::RenderQueue* rq = &rt->GetRenderQueue();
			if( m_started == true && m_isActive == true && GameObject::m_isShutdown == false && m_meshModel != nullptr )
			{
				rq->AddMesh( *this );
			}

			/*set the mesh's render group id*/
			Kiwi::RenderGroup* rqg = rq->FindRenderGroup( m_renderGroup );
			if( rqg != nullptr )
			{
				m_renderGroupID = rqg->id;
				m_viewportIndex = rqg->viewportIndex;

			} else
			{
				m_renderGroupID = 0;
				m_viewportIndex = 0;
			}
		}

		return true;
	}

	void Mesh::_UnsetRenderTarget()
	{
		/*get a pointer to the render target*/
		Kiwi::Renderer* renderer = _kGraphics.GetActiveRenderer();
		if( renderer == nullptr ) return;

		m_renderTarget = L"";
		m_renderTargetID = 0;
		m_renderGroupID = 0;
		m_viewportIndex = 0;

		Kiwi::RenderTarget* rt = renderer->FindRenderTarget( m_renderTarget );
		if( rt == nullptr ) return;

		/*remove the mesh from the render target's render queue*/
		Kiwi::RenderQueue* rq = &rt->GetRenderQueue();
		rq->RemoveMesh( *this );
	}

	void Mesh::Clear()
	{
		if( m_meshModel != nullptr )
		{
			m_meshModel->_UnlinkMesh( *this );
			m_meshModel->Free();
			m_meshModel = nullptr;
		}
	}

	bool Mesh::IntersectRay( const Kiwi::Vector3d& rayOrigin, const Kiwi::Vector3d& rayDirection, double maxDepth, std::vector<Kiwi::Mesh::Face>& collisions, int maxCollisions, bool includeBackFaces )
	{
		/*
		uses Möller–Trumbore intersection algorithm
		https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
		http://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
		*/

		int numCollisions = 0;

		if( m_meshModel == nullptr ) return false;

		Kiwi::Transform* entTransform = m_entity->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );
		if( entTransform == 0 )
		{
			return false;
		}

		Kiwi::Vector3d globalPos = entTransform->GetGlobalPosition();
		Kiwi::Vector3d scale = entTransform->GetScale();
		Kiwi::Vector3d maxPos = rayOrigin + (rayDirection * maxDepth);

		std::vector<Kiwi::Vertex> vertices = m_meshModel->GetVertices();

		if( m_entity != 0 && m_meshModel->GetPrimitiveTopology() == Kiwi::TRIANGLE_LIST )
		{
			for( unsigned int i = 0; i < vertices.size(); i += 3 )
			{

				if( (vertices[i].position.z + globalPos.z > maxPos.z && vertices[i + 1].position.z + globalPos.z > maxPos.z && vertices[i + 2].position.z + globalPos.z > maxPos.z) ||
					(vertices[i].position.z + globalPos.z < rayOrigin.z && vertices[i + 1].position.z + globalPos.z < rayOrigin.z && vertices[i + 2].position.z + globalPos.z < rayOrigin.z) )
				{
					continue;
				}

				Kiwi::Vector3d edge1, edge2;
				edge1 = vertices[i + 1].position - vertices[i].position; //v1 - v0
				edge2 = vertices[i + 2].position - vertices[i].position; //v2 - v0
				edge1 = edge1 * scale.x;
				edge2 = edge2 * scale.y;

				Kiwi::Vector3d phit = rayDirection.Cross( edge2 ); //point where ray intersects plane of triangle

				double determinant = edge1.Dot( phit );

				double t = 0.0;

				if( includeBackFaces )
				{
					// if the determinant is negative the triangle is backfacing
					// if the determinant is close to 0, the ray misses the triangle
					if( determinant < 0.000001 ) continue;

					Kiwi::Vector3d tVec = rayOrigin - ((vertices[i].position * scale.x) + globalPos); //origin - v0

					double u = tVec.Dot( phit );
					if( u < 0.0 || u > determinant ) continue;

					Kiwi::Vector3d qVec = tVec.Cross( edge1 );
					double v = rayDirection.Dot( qVec );
					if( v < 0.0 || u + v > determinant ) continue;

				} else
				{
					// ray and triangle are parallel if det is close to 0
					if( std::abs( determinant ) < 0.000001 ) continue;

					double invDet = 1.0 / determinant;

					Kiwi::Vector3d tVec = rayOrigin - ((vertices[i].position * scale.x) + globalPos); //origin - v0

					double u = tVec.Dot( phit ) * invDet;
					if( u < 0.0 || u > 1.0 ) continue;

					Kiwi::Vector3d qVec = tVec.Cross( edge1 );
					double v = rayDirection.Dot( qVec ) * invDet;
					if( v < 0.0 || u + v > 1.0 ) continue;
				}

				//if( t > 0.000001 )
				//{
					Face tri;
					tri.v1 = vertices[i].position;
					tri.v2 = vertices[i + 1].position;
					tri.v3 = vertices[i + 2].position;

					tri.n1 = vertices[i].normal;
					tri.n2 = vertices[i + 1].normal;
					tri.n3 = vertices[i + 2].normal;

					tri.i1 = m_meshModel->GetIndices()[i];
					tri.i2 = m_meshModel->GetIndices()[i + 1];
					tri.i3 = m_meshModel->GetIndices()[i + 2];

					tri.c1 = vertices[i].color;
					tri.c2 = vertices[i + 1].color;
					tri.c3 = vertices[i + 2].color;

					collisions.push_back( tri );

					if( maxCollisions > 0 && numCollisions > maxCollisions ) return (collisions.size() != 0);
				//}
			}
		}

		return (collisions.size() != 0);
	}

	bool Mesh::SetRenderGroup( const std::wstring& renderGroupName )
	{
		/*get a pointer to the render target*/
		Kiwi::Renderer* renderer = _kGraphics.GetActiveRenderer();
		if( renderer == nullptr ) return false;

		Kiwi::RenderTarget* rt = renderer->FindRenderTarget( m_renderTarget );
		if( rt == nullptr )
		{
			m_renderTarget = L"";
			m_renderTargetID = 0;
			m_renderGroupID = 0;
			m_viewportIndex = 0;
			return false;
		}

		/*add the mesh to the render target's render queue*/
		Kiwi::RenderQueue* rq = &rt->GetRenderQueue();

		/*set the mesh's render group id*/
		Kiwi::RenderGroup* rqg = rq->FindRenderGroup( renderGroupName );
		if( rqg != nullptr )
		{
			m_renderGroupID = rqg->id;
			m_viewportIndex = rqg->viewportIndex;
			m_renderGroup = renderGroupName;

		} else
		{
			m_renderGroupID = 0;
			m_viewportIndex = 0;
		}

		return true;
	}

	bool Mesh::SetRenderTarget( const std::wstring& renderTarget )
	{
		if( m_renderTarget.compare( renderTarget ) == 0 ) return true;		

		if( m_renderTarget.size() > 0 ) this->_UnsetRenderTarget();

		return this->_SetRenderTarget( renderTarget );
	}

	void Mesh::SetModel( const std::wstring& modelName, bool copyOriginal )
	{
		/*either fetch the asset or add the mesh as a listener for the model if it isn't loaded yet*/
		m_modelName = modelName;

		if( m_modelName.size() > 0 )
		{
			m_copyModelOnFetch = copyOriginal;
			this->FetchAsset( L"Model", m_modelName );

		} else
		{
			this->SetModel( nullptr );
		}
	}

	void Mesh::SetModel( Kiwi::IModel* model, bool copyOriginal )
	{
		if( m_meshModel != nullptr && (model == nullptr || model->GetAssetName().compare(m_meshModel->GetAssetName()) != 0) )
		{
			if( m_addedToRenderTargetQueue ) this->_RemoveFromRenderTargetQueue();
			m_meshModel->_UnlinkMesh( *this );
			m_meshModel->Free();
			m_meshModel = nullptr;
		}

		m_meshModel = model;
		if( m_meshModel == nullptr ) return;
		
		/*if the model isn't shared and copyOriginal is true, create a new copy exclusive to this mesh*/
		if( m_meshModel->IsShared() == false && copyOriginal == true )
		{
			m_meshModel = _kAssetManager.CopyAsset<Kiwi::IModel>( L"Model", model->GetAssetName(), m_objectName + L"/Model" );
		}
		if( m_meshModel == nullptr ) return;

		m_meshModel->_LinkMesh( *this );
		m_meshModel->Reserve();
		m_modelName = m_meshModel->GetAssetName();
		
		this->_AddToRenderTargetQueue();
	}


	//----Static member functions

	//Kiwi::Mesh* Mesh::Primitive( std::wstring name, Kiwi::Scene& scene, Kiwi::Mesh::PRIMITIVE_TYPE primitiveType )
	//{
	//	switch( primitiveType )
	//	{
	//		case Mesh::PRIMITIVE_TYPE::QUAD:
	//			{
	//				float xPos = 0.5;
	//				float yPos = 0.5;

	//				std::vector<Kiwi::Vector3d> meshVertices = {
	//					Kiwi::Vector3d( -xPos, yPos, 0.0f ),
	//					Kiwi::Vector3d( xPos, yPos, 0.0f ),
	//					Kiwi::Vector3d( -xPos, -yPos, 0.0f ),
	//					Kiwi::Vector3d( -xPos, -yPos, 0.0f ),
	//					Kiwi::Vector3d( xPos, yPos, 0.0f ),
	//					Kiwi::Vector3d( xPos, -yPos, 0.0f )
	//				};

	//				std::vector<Kiwi::Vector3d> meshNormals = {
	//					Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
	//					Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
	//					Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
	//					Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
	//					Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
	//					Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
	//				};

	//				std::vector<Kiwi::Vector2d> meshUVs = {
	//					Kiwi::Vector2d( 0.0, 0.0 ),
	//					Kiwi::Vector2d( 1.0, 0.0 ),
	//					Kiwi::Vector2d( 0.0, 1.0 ),
	//					Kiwi::Vector2d( 0.0, 1.0 ),
	//					Kiwi::Vector2d( 1.0, 0.0 ),
	//					Kiwi::Vector2d( 1.0, 1.0 ),
	//				};

	//				Kiwi::Mesh* quad = new Kiwi::Mesh( name, scene, meshVertices, meshUVs, meshNormals );
	//				quad->CreateSubmesh( Kiwi::Material(), 0, 5 );

	//				return quad;
	//			}
	//		case Mesh::PRIMITIVE_TYPE::CUBE:
	//			{
	//				double xPos = 0.5;
	//				double yPos = 0.5;
	//				double zPos = 0.5;

	//				std::vector<Kiwi::Vector3d> meshVertices = {
	//					//back face
	//					Kiwi::Vector3d( -xPos, yPos, -zPos ),
	//					Kiwi::Vector3d( xPos, yPos, -zPos ),
	//					Kiwi::Vector3d( -xPos, -yPos, -zPos ),
	//					Kiwi::Vector3d( -xPos, -yPos, -zPos ),
	//					Kiwi::Vector3d( xPos, yPos, -zPos ),
	//					Kiwi::Vector3d( xPos, -yPos, -zPos ),
	//					//right face
	//					Kiwi::Vector3d( xPos, yPos, -zPos ),
	//					Kiwi::Vector3d( xPos, yPos, zPos ),
	//					Kiwi::Vector3d( xPos, -yPos, -zPos ),
	//					Kiwi::Vector3d( xPos, -yPos, -zPos ),
	//					Kiwi::Vector3d( xPos, yPos, zPos ),
	//					Kiwi::Vector3d( xPos, -yPos, zPos ),
	//					//front face
	//					Kiwi::Vector3d( xPos, yPos, zPos ),
	//					Kiwi::Vector3d( -xPos, yPos, zPos ),
	//					Kiwi::Vector3d( xPos, -yPos, zPos ),
	//					Kiwi::Vector3d( xPos, -yPos, zPos ),
	//					Kiwi::Vector3d( -xPos, yPos, zPos ),
	//					Kiwi::Vector3d( -xPos, -yPos, zPos ),
	//					//left face
	//					Kiwi::Vector3d( -xPos, yPos, zPos ),
	//					Kiwi::Vector3d( -xPos, yPos, -zPos ),
	//					Kiwi::Vector3d( -xPos, -yPos, zPos ),
	//					Kiwi::Vector3d( -xPos, -yPos, zPos ),
	//					Kiwi::Vector3d( -xPos, yPos, -zPos ),
	//					Kiwi::Vector3d( -xPos, -yPos, -zPos ),
	//					//top face
	//					Kiwi::Vector3d( -xPos, yPos, zPos ),
	//					Kiwi::Vector3d( xPos, yPos, zPos ),
	//					Kiwi::Vector3d( -xPos, yPos, -zPos ),
	//					Kiwi::Vector3d( -xPos, yPos, -zPos ),
	//					Kiwi::Vector3d( xPos, yPos, zPos ),
	//					Kiwi::Vector3d( xPos, yPos, -zPos ),
	//					//bottom face
	//					Kiwi::Vector3d( -xPos, -yPos, -zPos ),
	//					Kiwi::Vector3d( xPos, -yPos, -zPos ),
	//					Kiwi::Vector3d( -xPos, -yPos, zPos ),
	//					Kiwi::Vector3d( -xPos, -yPos, zPos ),
	//					Kiwi::Vector3d( xPos, -yPos, -zPos ),
	//					Kiwi::Vector3d( xPos, -yPos, zPos )
	//				};

	//				std::vector<Kiwi::Vector3d> meshNormals = {
	//					//back face
	//					Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
	//					Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
	//					Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
	//					Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
	//					Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
	//					Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
	//					//right face
	//					Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
	//					Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
	//					Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
	//					Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
	//					Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
	//					Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
	//					//front face
	//					Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
	//					Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
	//					Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
	//					Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
	//					Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
	//					Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
	//					//left face
	//					Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
	//					Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
	//					Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
	//					Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
	//					Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
	//					Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
	//					//top face
	//					Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
	//					Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
	//					Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
	//					Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
	//					Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
	//					Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
	//					//bottom face
	//					Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
	//					Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
	//					Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
	//					Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
	//					Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
	//					Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
	//				};

	//				std::vector<Kiwi::Vector2d> meshUVs = {
	//					//back face
	//					Kiwi::Vector2d( 0.0, 0.0 ),
	//					Kiwi::Vector2d( 1.0, 0.0 ),
	//					Kiwi::Vector2d( 0.0, 1.0 ),
	//					Kiwi::Vector2d( 0.0, 1.0 ),
	//					Kiwi::Vector2d( 1.0, 0.0 ),
	//					Kiwi::Vector2d( 1.0, 1.0 ),
	//					//right face
	//					Kiwi::Vector2d( 0.0, 0.0 ),
	//					Kiwi::Vector2d( 1.0, 0.0 ),
	//					Kiwi::Vector2d( 0.0, 1.0 ),
	//					Kiwi::Vector2d( 0.0, 1.0 ),
	//					Kiwi::Vector2d( 1.0, 0.0 ),
	//					Kiwi::Vector2d( 1.0, 1.0 ),
	//					//front face
	//					Kiwi::Vector2d( 0.0, 0.0 ),
	//					Kiwi::Vector2d( 1.0, 0.0 ),
	//					Kiwi::Vector2d( 0.0, 1.0 ),
	//					Kiwi::Vector2d( 0.0, 1.0 ),
	//					Kiwi::Vector2d( 1.0, 0.0 ),
	//					Kiwi::Vector2d( 1.0, 1.0 ),
	//					//left face
	//					Kiwi::Vector2d( 0.0, 0.0 ),
	//					Kiwi::Vector2d( 1.0, 0.0 ),
	//					Kiwi::Vector2d( 0.0, 1.0 ),
	//					Kiwi::Vector2d( 0.0, 1.0 ),
	//					Kiwi::Vector2d( 1.0, 0.0 ),
	//					Kiwi::Vector2d( 1.0, 1.0 ),
	//					//top face
	//					Kiwi::Vector2d( 0.0, 0.0 ),
	//					Kiwi::Vector2d( 1.0, 0.0 ),
	//					Kiwi::Vector2d( 0.0, 1.0 ),
	//					Kiwi::Vector2d( 0.0, 1.0 ),
	//					Kiwi::Vector2d( 1.0, 0.0 ),
	//					Kiwi::Vector2d( 1.0, 1.0 ),
	//					//bottom face
	//					Kiwi::Vector2d( 0.0, 0.0 ),
	//					Kiwi::Vector2d( 1.0, 0.0 ),
	//					Kiwi::Vector2d( 0.0, 1.0 ),
	//					Kiwi::Vector2d( 0.0, 1.0 ),
	//					Kiwi::Vector2d( 1.0, 0.0 ),
	//					Kiwi::Vector2d( 1.0, 1.0 ),
	//				};

	//				Kiwi::Mesh* cube = new Kiwi::Mesh( name, scene, meshVertices, meshUVs, meshNormals );
	//				cube->CreateSubmesh( Kiwi::Material(), 0, 35 );

	//				return cube;
	//			}
	//		default: return 0;
	//	}
	//}

};