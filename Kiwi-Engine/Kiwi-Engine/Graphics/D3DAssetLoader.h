#ifndef _KIWI_D3DASSETLOADER_H_
#define _KIWI_D3DASSETLOADER_H_

#include <mutex>
#include <thread>
#include <string>

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace Kiwi
{

	class Texture;
	class Shader;
	class Renderer;

	class D3DAssetLoader
	{
	protected:

		ID3D11Device* m_device;
		ID3D11DeviceContext* m_deviceContext;

		std::mutex m_deviceMutex;
		std::mutex m_deviceContextMutex;

	public:

		D3DAssetLoader();
		~D3DAssetLoader();

		Kiwi::Texture* LoadTexture( Kiwi::Renderer* renderer, std::wstring textureName, std::wstring filename );

		ID3D11Device* GetDevice()const { return m_device; }
		ID3D11DeviceContext* GetDeviceContext()const { return m_deviceContext; }

	};

};

#endif